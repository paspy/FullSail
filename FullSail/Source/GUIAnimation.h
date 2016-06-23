#pragma once

#include <vector>


namespace G2D
{
	class Entity2D;
	class GUITexture;
}

typedef std::unordered_map<std::string, G2D::Entity2D*> GUIMap;


namespace G2D
{
	class GUIFrame
	{
	public:
		GUIFrame(void) : m_pTexture(nullptr), m_fDuration(0.0f) {}
		GUIFrame(GUITexture * texture, float duration) : m_pTexture(texture), m_fDuration(duration) {}
		~GUIFrame();

		// Mutators
		void Texture(GUITexture* texture) { m_pTexture = texture; }
		void Duration(float duration) { m_fDuration = duration; }

		// Accessors
		GUITexture* Texture(void) const { return m_pTexture; }
		float Duration(void) const { return m_fDuration; }

	private:
		GUITexture*		m_pTexture	= nullptr;
		float			m_fDuration	= 0.0f;
	};


	class GUIAnimation
	{
		struct GUIAnimTimeStamp
		{
			int		m_nCurrFrame	= 0;
			float	m_fCurrDuration	= 0.0f;

			GUIAnimTimeStamp(void) : m_nCurrFrame(0), m_fCurrDuration(0.0f) {}
		};

	public:
		GUIAnimation(void);
		GUIAnimation(GUIFrame * guitextures, unsigned int numtextures, Vector2 position = { 0.0f, 0.0f }, bool looping = false, bool playing = false, bool showing = true);
		GUIAnimation(std::vector<GUIFrame*> guitextures, Vector2 position = { 0.0f, 0.0f }, bool looping = false, bool playing = false, bool showing = true);
		~GUIAnimation(void);

		// interfaces
		void Render(void);
		void Update(float dt);

		void LoadGUIAnimation(std::string guianimfilename, GUIMap& hudelems);
		void AddFrame(GUIFrame * frame);
		void RemoveFrame(GUIFrame * frame);
		void RemoveAllFrames(void);

		void ResetAnimation(void);
		void RestartAnimation(void);
		void PlayAnimation(void);
		void PauseAnimation(void);
		void ShowAnimation(void);
		void HideAnimation(void);

		bool EndOfAnimation(void) { return (m_sTimeStamp.m_nCurrFrame >= m_vecFrames.size()) ? true : false; }

		// Mutators
		void Position(Vector2 position) { m_vPosition = position; }
		void Looping(bool looping) { m_bLooping = looping; }

		// Accessors
		std::vector<GUIFrame*> Frames(void) const { m_vecFrames; }
		Vector2 Position(void) const { return m_vPosition; }
		bool Looping(void) const { return m_bLooping; }
	
	private:
		std::vector<GUIFrame*>		m_vecFrames;	// animation frames
		Vector2						m_vPosition;	// xy position (anchor point)
		GUIAnimTimeStamp			m_sTimeStamp;	// controls the animation process
		bool						m_bLooping;		// looping animation
		bool						m_bPlaying;		// playing animation
		bool						m_bShowing;		// showing animation
	};


	class GUIAnimationMap
	{
		typedef std::map<std::string, GUIAnimation*> AnimationMap;

	public:
		GUIAnimationMap(void);
		~GUIAnimationMap(void);

		// interfaces
		void RenderAll(void);
		void UpdateAll(float dt);

		void LoadGUIAnimation(std::string guianimfilename, GUIMap& hudelems);
		void AddAnimation(std::string name, GUIAnimation* animation);
		void RemoveAnimation(std::string name);
		void RemoveAllAnimation(void);

		// Accessors
		GUIAnimation* Animation(std::string animation) { return m_mapAnimations[animation]; }
		//GUIAnimation* Animation(std::string animation) const { return m_mapAnimations[animation]; }
		//AnimationMap Animations(void) const { return m_mapAnimations; }
	
	private:
		AnimationMap m_mapAnimations;
	};
}

