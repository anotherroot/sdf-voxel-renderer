#ifndef ARC_TEXTURE
#define ARC_TEXTURE
#include "string"
namespace arc::tex {

typedef unsigned int TextureID;
// FilterType
typedef unsigned int FilterType;
const FilterType NEAREST = 0x0001;
const FilterType LINEAR = 0x0002;

typedef unsigned int WrapType;
const WrapType CLAMP_TO_EDGE = 0x0001;
const WrapType REPEAT = 0x0002;

// base format
typedef unsigned int BaseFormatType;
const BaseFormatType DEPTH_COMPONENT = 0x0001;
const BaseFormatType DEPTH_STENCIL = 0x0002;
const BaseFormatType RED = 0x0003;
const BaseFormatType RG = 0x0004;
const BaseFormatType RGB = 0x0005;
const BaseFormatType RGBA = 0x0006;

// data type
typedef unsigned int DataType;
const DataType UNSIGNED_BYTE = 0x0001;
const DataType BYTE = 0x0002;
const DataType UNSIGNED_SHORT = 0x0003;
const DataType SHORT = 0x0004;
const DataType UNSIGNED_INT = 0x0005;
const DataType INT = 0x0006;
const DataType HALF_FLOAT = 0x0007;
const DataType FLOAT = 0x0008;

//===============================================================>
void Create2D(TextureID &id, int width, int height, const BaseFormatType format,
              const DataType type, void *data);
void Create3D(TextureID &id, int width, int height, int depth,
              const BaseFormatType format, const DataType type, void *data);
void Dispose(TextureID id);
void Dispose();
//===============================================================>
void Bind(TextureID id, unsigned int slot);
void SetFilter2D(TextureID id, const FilterType type);
void SetWrap2D(TextureID id, WrapType type);
void Resize2D(TextureID id, int width, int height, const BaseFormatType format,
              unsigned int type, void *data);
void Resize2D(TextureID id, int width, int height);
//===============================================================>
void SetFilter3D(TextureID id, const FilterType type);
void SetWrap3D(TextureID id, const WrapType type);
void Resize3D(TextureID id, int width, int height, int depth,
              const BaseFormatType format, const DataType type, void *data);
//===============================================================>
void BindImage(TextureID id, unsigned int unit,bool layerd = false);
unsigned int GetID(TextureID id);
} // namespace arc::tex
#endif
