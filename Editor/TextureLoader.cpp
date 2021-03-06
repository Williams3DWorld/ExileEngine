#include "TextureLoader.h"
#include "stb_image.h"

uint32_t TextureLoader::LoadTexture2D(const char* uri, GLint wrap, GLint filter)
{
	uint32_t tex_id = 0;
	TextureData td = ImageLoader::LoadDataFromFile(uri);

#ifdef _DEBUG
	if (td.data == NULL)
	{
		ExCore::Logger::PrintErr("Failed to generate texture!");
		return 0;
	}
#endif

	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, td.width, td.height, 0, GL_RGB, GL_UNSIGNED_BYTE, td.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(td.data);

	return tex_id;
}
