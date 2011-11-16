#ifndef __LW_SOUND_H__
#define __LW_SOUND_H__

namespace lw{
	
	//class Sound{
	//public:
	//	static void init();
	//	static void quit();
	//	static void clear();
	//	static void main(float dt);
	//	static Sound* create(const char* fileName, bool loop);
	//	static Sound* create(const char* attackFileName, const char* loopFileName);
	//	
	//	virtual void play() = 0;
	//	virtual void stop() = 0;
	//	virtual void setVolume(float v) = 0;

	//	virtual ~Sound() {}
	//};

	class SoundMgr;

	struct SoundBuffer{
		unsigned int id;
		int refCount;
		const char* fileName;
	};
	struct SoundSource{
		unsigned int source;
		bool used;
	};

	//single instance
	class Sound{
	public:
		~Sound();
		bool play();
		void stop();
		void setLoop(bool b){
			_isLoop = b;
		}
		bool isPlaying(){
			return _source != 0;
		}
		void setVolume(float volume){
			_volume = volume;
		}
		void setPitch(float pitch){
			_pitch = pitch;
		}
		void setSecOffset(float offset){
			_secOffset = offset;
		}
		void setPitchNow(float pitch);
		void setVolumeNow(float volume);
		void setSecOffsetNow(float offset);
		float getSecOffset();

	private:
		Sound(SoundBuffer* pBuffer);
		Sound(SoundBuffer* pAttackBuffer, SoundBuffer* pLoopBuffer);
		void onPlayEnd();

		bool _isLoop;
		SoundBuffer* _pBuffer;
		SoundBuffer* _pLoopBuffer;
		unsigned int _source;
		float _volume;
		float _pitch;
		float _secOffset;

		friend class SoundMgr;
	};

	//multi instance
	class Sounds{
	public:
		~Sounds();
		Sound* play();
		void stop();
		void setVolume(float volume);
		void setPitch(float pitch);
		void setPitchNow(float pitch);

	private:
		Sounds(const char* fileName, int instanceNum, bool isPath, bool& ok);
		std::vector<Sound*> _sounds;
		int _currIdx;
		friend class SoundMgr;
	};

	class SoundMgr : public Singleton<SoundMgr>{
	public:
		SoundMgr(int maxSources);
		~SoundMgr();
		Sound* createSound(const char* fileName, bool isPath = false);
		Sound* createSound(const char* attackFileName, const char* loopFileName, bool isPath = false);
		Sounds* createSounds(const char* fileName, int instanceNum, bool isPath = false);
		void main(float dt);

	private:
		void releaseSoundBuffer(SoundBuffer* pSoundBuffer);
		unsigned int getFreeSource();
		SoundBuffer* getBuffer(const char* fileName, bool isPath);

		std::map<std::string, SoundBuffer*> _bufferMap;		
		std::vector<SoundSource> _sources;
		std::list<Sound*> _noLoopPlayingSounds;

		friend class Sound;
	};

	

} //namespace lw

#endif //__LW_SOUND_H__