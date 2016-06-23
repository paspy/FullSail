#ifndef __EXPORTER_HEADER_H__
#define __EXPORTER_HEADER_H__
//--------------------------------------------------------------------------------
//THis file should be included in the loader and the exporter
#include <memory.h>
#include <stdint.h>
#include <sys/stat.h>

#include <time.h>
//--------------------------------------------------------------------------------

//Change this when any changes are done to the struct ExporterHeader
#define EXPORTER_VERSION_NUMBER 1
//--------------------------------------------------------------------------------
namespace FileInfo {
	enum class FILE_TYPES : int8_t { MESH, BIND_POSE, ANIMATION, NAV_MESH };
	enum class MODEL_TYPES : int8_t { COLOR, TEXTURE, TEXTURE_LIT, NORMALMAP, NORMALMAP_ANIMATED, BASIC, MAX_TYPES };
	enum class INDEX_TYPES : int8_t { INDEX32, INDEX16, TRI_STRIP };

	struct ExporterHeader {
		union {
			struct {
				uint32_t		numPoints;
				uint32_t		numIndex;
				unsigned int	vertSize;
				MODEL_TYPES		modelType;
				INDEX_TYPES		index;
			}mesh;
			struct {
				uint32_t		numBones;
			}bind;
			struct {
				uint32_t		numBones;
				uint32_t		numFrames;
				float			startTime;
				float			endTime;
			}anim;
			struct {
				uint32_t		totalSize;
				uint32_t		rowSize;
				uint32_t		numTriangles;
				uint32_t		elementSize;
			}navMesh;
		};
		FILE_TYPES		file;
		unsigned int	version;
		//Do a check to make sure the file has not been updated since you last changed it
		time_t		updateTime;

		//Used for reading in the header
		ExporterHeader() {}
		//used for writing out the header
		ExporterHeader(FILE_TYPES _type) {
			memset(this, 0, sizeof(ExporterHeader));
			version = EXPORTER_VERSION_NUMBER;
			file = _type;
			    
			//https://msdn.microsoft.com/query/dev12.query?appId=Dev12IDEF1&l=EN-US&k=k(wchar%2Fstat);k(stat);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
			struct stat fileProperty;
			int retVal = stat("./Resources/RenderObjects/box.txt", &fileProperty);   
			//save last modified time to the file
			updateTime = fileProperty.st_mtime;
			//If you want to see the time in a more readable format
			//struct tm time;               // create a time structure
			//gmtime_s(&time, &(attrib.st_mtime)); // Get the last modified time and put it into the time structure
		}
	};
};
//--------------------------------------------------------------------------------
#endif //__EXPORTER_HEADER_H__