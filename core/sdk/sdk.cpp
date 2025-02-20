#include "sdk.h"

#include "../utils/utils.h"
#include "../sdk/interfaces/memory.h"
#include "..\sdk\interfaces\filesystem.h"
#include "..\sdk\interfaces\engineclient.h"
#include "..\sdk\interfaces\cvar.h"
#include "..\sdk\interfaces\globalvars.h"
#include "..\sdk\interfaces\inputsystem.h"
#include "..\sdk\interfaces\schemasystem.h"
#include "..\sdk\addresses\entity.h"


#define GAME_RESOURCE_SERVICE_CLIENT xorstr_("GameResourceServiceClientV00")
#define SOURCE2_CLIENT xorstr_("Source2Client00") // ISourceClient002 
#define SCHEMA_SYSTEM xorstr_("SchemaSystem_00")
#define INPUT_SYSTEM_VERSION xorstr_("InputSystemVersion00")
#define SOURCE2_ENGINE_TO_CLIENT xorstr_("Source2EngineToClient00")
#define ENGINE_CVAR xorstr_("VEngineCvar00")
#define LOCALIZE xorstr_("Localize_00")
#define NETWORK_CLIENT_SERVICE xorstr_("NetworkClientService_001")
#define MATERIAL_SYSTEM2 xorstr_("VMaterialSystem2_00")
#define RENDERSYS_SYSTEM xorstr_("RenderGameSystem_00")
#define RESOURCE_SYSTEM xorstr_("ResourceSystem013")
#define RESOURCE_HANDLE_UTILS xorstr_("ResourceHandleUtils001")

using InstantiateInterfaceFn = void* (*)();
class CInterfaceReg
{
public:
    InstantiateInterfaceFn m_create_fn;
    const char* m_name;
    CInterfaceReg* m_next;
};

inline const CInterfaceReg* GetInterfaces(const char* module_name)
{
    // get the handle of the specified module
    const HMODULE module_base = GetModuleHandleA(module_name);
    if (module_base == nullptr)
        return nullptr;

    // get the address of the symbol "CreateInterface" from the module
    const auto symbol = reinterpret_cast<std::uintptr_t>(GetProcAddress(module_base, xorstr_("CreateInterface")));

    // calculate the address of the list of interface registrations based on the symbol address
    const std::uintptr_t list = symbol + *reinterpret_cast<std::int32_t*>(symbol + 3) + 7;

    // return a pointer to the list of interface registrations
    return *reinterpret_cast<CInterfaceReg**>(list);
}
#define PARTICLE_SYSTEM_MANAGER X("ParticleSystemMgr00") //ParticleSystemMgr003
#define PARTICLES_DLL X(L"particles.dll")

template <typename T = void*>
T* GetInterface(const char* module_name, const char* interface_partial_version)
{
    // iterate through the list of interface registrations obtained from the module
    for (const CInterfaceReg* current = GetInterfaces(module_name); current; current = current->m_next)
    {
        // check if the name of the current interface contains the specified partial version string
        if (std::string_view(current->m_name).find(interface_partial_version) != std::string_view::npos)
        {
            // if found, create an instance of the interface using its creation function
            return reinterpret_cast<T*>(current->m_create_fn());
        }
    }

    // log an error if the interface was not found
    CS2_LOG(xorstr_("failed to find interface in %s, scanned for %s\n"), module_name, interface_partial_version);
    return nullptr;
}

bool sdk::Init()
{
    bool success = true;

    {
        m_swap_chain = **reinterpret_cast<ISwapChainDx11***>(utils::ResolveRelativeAddress(utils::FindPattern("rendersystemdx11.dll", "66 0F 7F 0D ? ? ? ? 48 8B F7 66 0F 7F 05"), 0x4, 0x8));
        success &= (m_swap_chain != nullptr);
        // grab's d3d11 interfaces for later use
        if (m_swap_chain != nullptr){
            if (FAILED(m_swap_chain->m_swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&m_device))){
                return false;
            }
            else
                m_device->GetImmediateContext(&m_device_context);
        }
        success &= (m_device != nullptr && m_device_context != nullptr);
    }


    m_csgo_input = *reinterpret_cast<CCSGOInput**>((utils::ToAbsolute(utils::FindPattern(xorstr_("client.dll"), xorstr_("48 89 05 ? ? ? ? 0F 57 C0 0F 11 05 ? ? ? ?")) + 0x3)));
    success &= (m_csgo_input != nullptr); 
    const auto pTier0Handle = utils::base_handle(L"tier0.dll");
    if (pTier0Handle == nullptr)
        return false;

    m_mem_alloc = *reinterpret_cast<IMemAlloc**>(utils::export_address(pTier0Handle, xorstr_("g_pMemAlloc")));
    success &= (m_mem_alloc != nullptr);

    m_game_rules = *reinterpret_cast<C_CSGameRules**>(utils::ResolveRelativeAddress(utils::FindPattern(xorstr_("client.dll"), xorstr_("48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 8D 54 24 ? 45 33 C0")), 0x3, 0x7));

    success &= (m_game_rules != nullptr);
    m_input_system = GetInterface<IInputSystem>(xorstr_("inputsystem.dll"), xorstr_("InputSystemVersion00"));

    success &= (m_input_system != nullptr);
    m_particle_system = GetInterface<IParticleSystemMgr>(xorstr_("particles.dll"), xorstr_("ParticleSystemMgr00"));

    success &= (m_particle_system != nullptr);

    m_schema_system = GetInterface<ISchemaSystem>(xorstr_("schemasystem.dll"), xorstr_("SchemaSystem_00"));

    success &= (m_schema_system != nullptr);

    m_client = GetInterface<ISource2Client>(xorstr_("client.dll"), SOURCE2_CLIENT);

    success &= (m_client != nullptr);

    m_engine = GetInterface<IEngineClient>(xorstr_("engine2.dll"), SOURCE2_ENGINE_TO_CLIENT);
    success &= (m_engine != nullptr);

    m_cvar = GetInterface<IEngineCVar>(xorstr_("tier0.dll"), ENGINE_CVAR);
    success &= (m_cvar != nullptr);

    m_localize = GetInterface<CLocalize>(xorstr_("localize.dll"), LOCALIZE);
    success &= (m_localize != nullptr);

    m_game = GetInterface<IGameResourceService>(xorstr_("engine2.dll"), GAME_RESOURCE_SERVICE_CLIENT);
    success &= (m_game != nullptr);

    // @ida: #STR '%s:  %f tick(%d) curtime(%f) OnSequenceCycleChanged: %s : %d=[%s]'
    m_global_vars = *reinterpret_cast<CGlobalVarsBase**>(utils::ResolveRelativeAddress(utils::FindPattern("client.dll", xorstr_("48 89 15 ?? ?? ?? ?? 48 89 42")), 0x3, 0x7));
    success &= (m_global_vars != nullptr);
    debug(m_global_vars != nullptr);

    m_file_system = GetInterface<IFileSystem>(xorstr_("filesystem_stdio.dll"), xorstr_("VFileSystem017"));
    debug(m_file_system != nullptr);
    success &= (m_file_system != nullptr);

    m_material = GetInterface<IMaterialSystem2>(xorstr_("materialsystem2.dll"), MATERIAL_SYSTEM2);
    success &= (m_material != nullptr);
    debug(m_material != nullptr);

    m_resource = GetInterface<IResourceSystem>(xorstr_("resourcesystem.dll"), RESOURCE_SYSTEM);
    debug(m_resource != nullptr);

    if (m_resource != nullptr)
    {
        m_resource_utils = reinterpret_cast<CResourceHandleUtils*>(m_resource->QueryInterface(RESOURCE_HANDLE_UTILS));
        debug(m_resource_utils != nullptr);
    }
    // return status
    return success;
}
