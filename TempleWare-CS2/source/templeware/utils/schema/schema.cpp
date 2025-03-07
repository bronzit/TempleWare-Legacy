#include "schema.h"
#include <vector>
#include <algorithm>
#include <Shlobj.h>
#include <shlobj_core.h>


#include "../fnv1a/fnv1a.h"

#include "../math/utlstring/utlstring.h"
#include "../memory/Interface/Interface.h"

struct SchemaDumpedData_t
{
	uint32_t hashedName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};

static std::vector<SchemaDumpedData_t> dumped_data;

bool Schema::init(const char* ModuleName, int module_type)
{

    schema_system = I::Get<ISchemaSystem>("schemasystem.dll", "SchemaSystem_00");
	if (!schema_system)
		return false;

	CSchemaSystemTypeScope* pTypeScope = schema_system->FindTypeScopeForModule(ModuleName);
	if (pTypeScope == nullptr)
		return false;

	const int nTableSize = pTypeScope->hashClasses.Count();

	UtlTSHashHandle_t* pElements = new UtlTSHashHandle_t[nTableSize + 1U];
	const auto nElements = pTypeScope->hashClasses.GetElements(0, nTableSize, pElements);

	for (int i = 0; i < nElements; i++)
	{
		const UtlTSHashHandle_t hElement = pElements[i];

		if (hElement == 0)
			continue;

		CSchemaClassBinding* pClassBinding = pTypeScope->hashClasses[hElement];
		if (pClassBinding == nullptr)
			continue;

		SchemaClassInfoData_t* pDeclaredClassInfo;
		pTypeScope->FindDeclaredClass(&pDeclaredClassInfo, pClassBinding->szBinaryName);

		if (pDeclaredClassInfo == nullptr)
			continue;

		if (pDeclaredClassInfo->nFieldSize == 0)
			continue;

		for (auto j = 0; j < pDeclaredClassInfo->nFieldSize; j++)
		{
			SchemaClassFieldData_t* pFields = pDeclaredClassInfo->pFields;
			std::string szFieldClassBuffer = std::string(pDeclaredClassInfo->szName) + "->" + std::string(pFields[j].szName);
			
			dumped_data.emplace_back(hash_32_fnv1a_const(szFieldClassBuffer.c_str()), pFields[j].nSingleInheritanceOffset);
		}

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		printf("[Schema] ");

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		printf("Dumped Class: %s fields: %i \n", pDeclaredClassInfo->szName, pDeclaredClassInfo->nFieldSize);
	}

	delete[] pElements;

	return true;
}

std::uint32_t SchemaFinder::Get(const uint32_t hashedName)
{
	if (const auto it = std::ranges::find_if(dumped_data, [hashedName](const SchemaDumpedData_t& data)
		{ return data.hashedName == hashedName; });
		it != dumped_data.end())
		return it->uOffset;

	return 0U;
}

