// used: [win] winapi
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "config.h"
// used: getworkingpath
#include "../../helpers/crt/crt.h"
#include "../../helpers/binary/binary.h"
#include "../../helpers/crt/crt.h"
#define CS_CONFIGURATION_FILE_EXTENSION L".bin"
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <cstdio>   // For fopen, fwrite, fclose


// default configurations working path
static wchar_t wszConfigurationsPath[MAX_PATH];
namespace fs = std::filesystem;

#include <string>  // For std::wstring and std::string conversions
#include "../render/ui/menu.h"
#include <unordered_set>  // Include for std::unordered_set

// Define a set to track UUIDs that have already been added
static std::unordered_set<std::string> added_uuids;

#include <unordered_set>  // Include for std::unordered_set
 std::vector< KeyBind_t>  m_key_binds;

#pragma region config_user_data_type

std::size_t C::UserDataType_t::GetSerializationSize() const
{
	std::size_t nTotalDataSize = 0U;

	for (const UserDataMember_t& member : vecMembers)
		nTotalDataSize += sizeof(uint32_t[2]) + member.nDataSize;

	return nTotalDataSize;
}

#pragma endregion

#pragma region config_variable_object

void C::VariableObject_t::SetStorage(const void* pValue)
{
	// check is available to store value in the local storage
	if (this->nStorageSize <= sizeof(this->storage.uLocal))
	{
		memset(&this->storage.uLocal, 0U, sizeof(this->storage.uLocal));
		memcpy(&this->storage.uLocal, pValue, this->nStorageSize);
	}
	// otherwise use heap memory to store it
	else
	{
		debug(this->storage.pHeap != nullptr); // tried to access non allocated storage

		memset(this->storage.pHeap, 0U, this->nStorageSize);
		memcpy(this->storage.pHeap, pValue, this->nStorageSize);
	}
}

std::size_t C::VariableObject_t::GetSerializationSize() const
{
	std::size_t nSerializationSize = this->nStorageSize;

	// denote a custom serialization size when it different from the storage size
	switch (this->uTypeHash)
	{
	// lookup for array data type
	case hash_32_fnv1a_const("bool[]"):
	case hash_32_fnv1a_const("int[]"):
	case hash_32_fnv1a_const("unsigned int[]"):
	case hash_32_fnv1a_const("float[]"):
	case hash_32_fnv1a_const("char[][]"):
		// arrays also serialize their size
		nSerializationSize += sizeof(std::size_t);
		break;
	// lookup for user-defined data type
	default:
	{
		for (const UserDataType_t& userType : vecUserTypes)
		{
			if (userType.uTypeHash == this->uTypeHash)
			{
				nSerializationSize = sizeof(std::size_t) + userType.GetSerializationSize();
				break;
			}
		}
		break;
	}
	}

	return nSerializationSize;
}

#pragma endregion
#include <shlobj_core.h>

bool get_path(wchar_t* wszDestination)
{
	bool bSuccess = false;
	PWSTR wszPathToDocuments = nullptr;

	// get path to user documents
	if (SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &wszPathToDocuments)))
	{
		c_run_time::StringCat(c_run_time::StringCopy(wszDestination, wszPathToDocuments), xorstr_(L"\\Nizzware\\"));
		bSuccess = true;
		// create directory if it doesn't exist
		if (!CreateDirectoryW(wszDestination, nullptr))
		{
			if (::GetLastError() != ERROR_ALREADY_EXISTS)
			{
				bSuccess = false;
			}
		}
	}
	::CoTaskMemFree(wszPathToDocuments);

	return bSuccess;
}
bool C::Setup(const wchar_t* wszDefaultFileName)
{

	if (!get_path(wszConfigurationsPath))
		return false;

	c_run_time::StringCat(wszConfigurationsPath, xorstr_(L"settings\\"));

	// create directory if it doesn't exist
	if (!::CreateDirectoryW(wszConfigurationsPath, nullptr))
	{
		if (::GetLastError() != ERROR_ALREADY_EXISTS)
		{
			return false;
		}
	}
	// @note: define custom data types we want to serialize
	AddUserType(hash_32_fnv1a_const("KeyBind_t"),
	{ 
		UserDataMember_t{ hash_32_fnv1a_const("uKey"), hash_32_fnv1a_const("unsigned int"), &KeyBind_t::uKey },
		UserDataMember_t{ hash_32_fnv1a_const("nMode"), hash_32_fnv1a_const("int"), &KeyBind_t::nMode } 
	});

	AddUserType(hash_32_fnv1a_const("ColorPickerVar_t"),
	{
		UserDataMember_t{ hash_32_fnv1a_const("bRainbow"), hash_32_fnv1a_const("bool"), &ColorPickerVar_t::bRainbow },
		UserDataMember_t{ hash_32_fnv1a_const("flRainbowSpeed"), hash_32_fnv1a_const("float"), &ColorPickerVar_t::flRainbowSpeed },
		UserDataMember_t{ hash_32_fnv1a_const("colPrimary"), hash_32_fnv1a_const("Color_t"), &ColorPickerVar_t::colValue },
	});

	AddUserType(hash_32_fnv1a_const("TextOverlayVar_t"), 
	{ 
		UserDataMember_t{ hash_32_fnv1a_const("bEnable"), hash_32_fnv1a_const("bool"), &TextOverlayVar_t::bEnable },
		UserDataMember_t{ hash_32_fnv1a_const("flThickness"), hash_32_fnv1a_const("float"), &TextOverlayVar_t::flThickness },
		UserDataMember_t{ hash_32_fnv1a_const("colPrimary"), hash_32_fnv1a_const("Color_t"), &TextOverlayVar_t::colPrimary },
		UserDataMember_t{ hash_32_fnv1a_const("colOutline"), hash_32_fnv1a_const("Color_t"), &TextOverlayVar_t::colOutline }
	});

	AddUserType(hash_32_fnv1a_const("FrameOverlayVar_t"),
	{
		UserDataMember_t{ hash_32_fnv1a_const("bEnable"), hash_32_fnv1a_const("bool"), &FrameOverlayVar_t::bEnable },
		UserDataMember_t{ hash_32_fnv1a_const("flThickness"), hash_32_fnv1a_const("float"), &FrameOverlayVar_t::flThickness },
		UserDataMember_t{ hash_32_fnv1a_const("flRounding"), hash_32_fnv1a_const("float"), &FrameOverlayVar_t::flRounding },
		UserDataMember_t{ hash_32_fnv1a_const("colPrimary"), hash_32_fnv1a_const("Color_t"), &FrameOverlayVar_t::colPrimary },
		UserDataMember_t{ hash_32_fnv1a_const("colOutline"), hash_32_fnv1a_const("Color_t"), &FrameOverlayVar_t::colOutline } 
	});

	AddUserType(hash_32_fnv1a_const("BarOverlayVar_t"),
	{ 
		UserDataMember_t{ hash_32_fnv1a_const("bEnable"), hash_32_fnv1a_const("bool"), &BarOverlayVar_t::bEnable },
		UserDataMember_t{ hash_32_fnv1a_const("bGradient"), hash_32_fnv1a_const("bool"), &BarOverlayVar_t::bGradient },
		UserDataMember_t{ hash_32_fnv1a_const("bUseFactorColor"), hash_32_fnv1a_const("bool"), &BarOverlayVar_t::bUseFactorColor },
		UserDataMember_t{ hash_32_fnv1a_const("flThickness"), hash_32_fnv1a_const("float"), &BarOverlayVar_t::flThickness },
		UserDataMember_t{ hash_32_fnv1a_const("colPrimary"), hash_32_fnv1a_const("Color_t"), &BarOverlayVar_t::colPrimary },
		UserDataMember_t{ hash_32_fnv1a_const("colSecondary"), hash_32_fnv1a_const("Color_t"), &BarOverlayVar_t::colSecondary },
		UserDataMember_t{ hash_32_fnv1a_const("colBackground"), hash_32_fnv1a_const("Color_t"), &BarOverlayVar_t::colBackground },
		UserDataMember_t{ hash_32_fnv1a_const("colOutline"), hash_32_fnv1a_const("Color_t"), &BarOverlayVar_t::colOutline } 
	});

	// create default configuration
	if (!CreateFile(wszDefaultFileName))
		return false;

	// store existing configurations list
	Refresh();

	return true;
}

#pragma region config_main

void C::Refresh()
{
	// clear and free previous stored file names
	vecFileNames.clear();

	// make configuration files path filter
	wchar_t wszPathFilter[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszPathFilter, wszConfigurationsPath), (L"*" CS_CONFIGURATION_FILE_EXTENSION));

	// iterate through all files with our filter
	WIN32_FIND_DATAW findData;
	if (const HANDLE hFindFile = ::FindFirstFileW(wszPathFilter, &findData); hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			vecFileNames.push_back(new wchar_t[c_run_time::StringLength(findData.cFileName) + 1U]);
			c_run_time::StringCopy(vecFileNames.back(), findData.cFileName);

		} while (::FindNextFileW(hFindFile, &findData));

		::FindClose(hFindFile);
	}
}

void C::AddUserType(const uint32_t uTypeHash, const std::initializer_list<UserDataMember_t> vecUserMembers)
{
	if (vecUserMembers.size() == 0U)
		return;

	UserDataType_t userDataType;
	userDataType.uTypeHash = uTypeHash;

	for (const auto& userDataMember : vecUserMembers)
		userDataType.vecMembers.push_back(userDataMember);

	vecUserTypes.emplace_back(c_run_time::Move(userDataType));
}


bool C::SaveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (BIN::SaveVariable(wszFilePath, variable))
	{
		return true;
	}

	return false;
}

bool C::LoadFileVariable(const std::size_t nFileIndex, VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (BIN::LoadVariable(wszFilePath, variable))
	{ 
		return true;
	}

	return false;
}

bool C::RemoveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (BIN::RemoveVariable(wszFilePath, variable))
	{
		return true;
	}

	return false;
}

bool C::CreateFile(const wchar_t* wszFileName)
{
	const wchar_t* wszFileExtension = c_run_time::StringCharR(wszFileName, L'.');

	// get length of the given filename and strip out extension if there any
	const std::size_t nFileNameLength = (wszFileExtension != nullptr ? wszFileExtension - wszFileName : c_run_time::StringLength(wszFileName));
	wchar_t* wszFullFileName = new wchar_t[nFileNameLength + c_run_time::StringLength(CS_CONFIGURATION_FILE_EXTENSION) + 1U];

	// copy filename without extension
	wchar_t* wszFullFileNameEnd = c_run_time::StringCopyN(wszFullFileName, wszFileName, nFileNameLength);
	*wszFullFileNameEnd = L'\0';
	// append correct extension to the filename
	c_run_time::StringCat(wszFullFileNameEnd, xorstr_(CS_CONFIGURATION_FILE_EXTENSION));

	// add filename to the list
	vecFileNames.push_back(wszFullFileName);

	// create and save it by the index
	if (SaveFile(vecFileNames.size() - 1U)){
		return true;
	}

	return false;
}

bool C::SaveFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (BIN::SaveFile(wszFilePath))
	{
		return true;
	}

	return false;
}

bool C::LoadFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (BIN::LoadFile(wszFilePath))
	{
		return true;
	}

	return false;
}

void C::RemoveFile(const std::size_t nFileIndex)
{
	const wchar_t* wszFileName = vecFileNames[nFileIndex];

	// unable to delete default config
	if (c_run_time::StringCompare(wszFileName, xorstr_(CS_CONFIGURATION_DEFAULT_FILE_NAME  CS_CONFIGURATION_FILE_EXTENSION)) == 0)
	{
		return;
	}

	wchar_t wszFilePath[MAX_PATH];
	c_run_time::StringCat(c_run_time::StringCopy(wszFilePath, wszConfigurationsPath), wszFileName);

	if (::DeleteFileW(wszFilePath))
	{
		// erase and free filename from the list
		vecFileNames.erase(vecFileNames.cbegin() + nFileIndex);

	}
}

#pragma endregion

#pragma region config_get

std::size_t C::GetVariableIndex(const uint32_t uNameHash)
{
	for (std::size_t i = 0U; i < vecVariables.size(); i++)
	{
		if (vecVariables[i].uNameHash == uNameHash)
			return i;
	}

	return cfg_invalidate;
}

#pragma endregion

#pragma region config_user_types
void ColorPickerVar_t::UpdateRainbow()
{
	// @todo: improve + optimize this code
	// progress rainbow color
	if (this->bRainbow)
	{
		const float flTime = static_cast<float>(ImGui::GetTime());
		// create a rainbow color with copied alpha
		float arrRainbowColors[] = {
			sin(flTime * this->flRainbowSpeed) * 0.5f + 0.5f,
			sin(flTime * this->flRainbowSpeed * math::_PI / 3) * 0.5f + 0.5f,
			sin(flTime * this->flRainbowSpeed * math::_PI / 3) * 0.5f + 0.5f,
			this->colValue.Base<COLOR_A>()
		};

		// set the rainbow color
		this->colValue = Color_t::FromBase4(arrRainbowColors);
	}
}
