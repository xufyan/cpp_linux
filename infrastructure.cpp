#include "infrastructure.h"

using namespace ::azure::storage::cpp_linux;

namespace azure {
	namespace storage {
		namespace cpp_linux {
			void SetResponse(LinuxFileResponse& response, bool success, std::string message) {
				response.Success = success;
				response.ResponseMessage = message;
			}

			LinuxFileException GetException(std::string errorMessage, OperationType::type type) {
				LinuxFileException linuxFileException;
				linuxFileException.ErrorMessage = errorMessage;
				linuxFileException.Type = type;
				return linuxFileException;
			}

			std::string IntToString(int64_t integer) {
				std::string result;
				std::ostringstream convert;
				convert << integer;
				result = convert.str();
				return result;
			}

			int GetFileFlag(LinuxFileAccess::type fileAccess, LinuxFileMode::type fileMode) {
				int flag = 0;
				switch (fileAccess) {
				case LinuxFileAccess::Read:
					flag |= O_RDONLY;
					break;
				case LinuxFileAccess::Write:
					flag |= O_WRONLY;
					break;
				case LinuxFileAccess::ReadWrite:
				default:
					flag |= O_RDWR;
					break;
				}
				switch (fileMode) {
				case LinuxFileMode::Append:
					flag |= O_APPEND;
					break;
				case LinuxFileMode::OpenOrCreate:
					flag |= O_CREAT;
				case LinuxFileMode::CreateNew:
					flag |= O_CREAT | O_EXCL;
				case LinuxFileMode::Truncate:
					flag |= O_TRUNC;
					break;
				case LinuxFileMode::Open:
				default:
					break;
				}
				return flag;
			}
		}
	}
}