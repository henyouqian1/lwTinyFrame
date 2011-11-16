#ifndef __LW_RESOURCE_HOLDER_H__
#define __LW_RESOURCE_HOLDER_H__

namespace lw{

	class Sprite;
	class Font;

	class ResourceHolder{
	public:
		~ResourceHolder();
		void addTexture(const char* fileName);
		void addFont(const char* fileName);
		void clear();

	private:
		std::vector<lw::Sprite*> _textures;
		std::vector<lw::Font*> _fonts;
	};

} //namespace lw

#endif //__RESOURCE_HOLDER_H__