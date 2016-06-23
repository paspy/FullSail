#include "pch.h"
#include "GUIAnimation.h"
#include "GUITexture.h"
#include "Log.h"

namespace G2D
{
	//////////////////////
	// GUIFrame
	//////////////////////

	GUIFrame::~GUIFrame(void)
	{
		m_pTexture = nullptr;
		m_fDuration = 0.0f;
	}



	//////////////////////
	// GUIAnimation
	//////////////////////

	GUIAnimation::GUIAnimation(void)
	{
		m_vPosition = Vector2(0.0f, 0.0f);
		m_sTimeStamp = GUIAnimTimeStamp();
		m_bLooping = false;
		m_bPlaying = false;
		m_bShowing = true;
	}


	GUIAnimation::GUIAnimation(GUIFrame * guitextures, unsigned int numtextures, Vector2 position, bool looping, bool playing, bool showing)
	{
		// add frames
		for (unsigned int i = 0; i < numtextures; i++)
			AddFrame(&guitextures[i]);

		m_vPosition = position;
		m_sTimeStamp = GUIAnimTimeStamp();
		m_bLooping = looping;
		m_bPlaying = playing;
		m_bShowing = showing;
	}


	GUIAnimation::GUIAnimation(std::vector<GUIFrame*> guitextures, Vector2 position, bool looping, bool playing, bool showing)
	{
		// # of frames
		size_t numtextures = guitextures.size();

		// add frames
		for (size_t i = 0; i < numtextures; i++)
			AddFrame(guitextures[i]);

		m_vPosition = position;
		m_sTimeStamp = GUIAnimTimeStamp();
		m_bLooping = looping;
		m_bPlaying = playing;
		m_bShowing = showing;
	}


	GUIAnimation::~GUIAnimation(void)
	{
		RemoveAllFrames();
	}


	void GUIAnimation::Render(void)
	{
		// # of frames
		size_t numframes = m_vecFrames.size();

		// turn off all frames
		for (size_t i = 0; i < numframes; i++)
			m_vecFrames[i]->Texture()->Active(false);

		// not showing?
		if (m_bShowing == false)
			return;

		// set position
		//m_vecFrames[m_sTimeStamp.m_nCurrFrame]->Texture()->Position(m_vPosition);

		// turn on current frame
		m_vecFrames[m_sTimeStamp.m_nCurrFrame]->Texture()->Active(true);
	}


	void GUIAnimation::Update(float dt)
	{
		// not showing?
		if (m_bShowing == false)
			return;

		// not playing?
		if (m_bPlaying == false)
			return;

		// current animation frame
		GUIFrame* pCurrFrame = m_vecFrames[m_sTimeStamp.m_nCurrFrame];

		// frame timer
		m_sTimeStamp.m_fCurrDuration += dt;

		// next frame
		if (m_sTimeStamp.m_fCurrDuration >= pCurrFrame->Duration())
		{
			// reset timer
			m_sTimeStamp.m_fCurrDuration = 0.0f;

			// go to next frame in animation
			m_sTimeStamp.m_nCurrFrame++;

			// end of animation?
			if (m_sTimeStamp.m_nCurrFrame == m_vecFrames.size())
			{
				// loop animation?
				(m_bLooping == true)
					? m_sTimeStamp.m_nCurrFrame = 0	// restart animation
					: --m_sTimeStamp.m_nCurrFrame;	// stop on the last frame
			}
		}
	}


	void GUIAnimation::LoadGUIAnimation(std::string guianimfilename, GUIMap& hudelems)
	{
		// create file path with filename
		std::string file		= guianimfilename + ".xml";
		std::string filepath	= "../../FullSail/Resources/GUI/Configs/" + file;


		// Load xml from filepath
		tinyxml2::XMLDocument xmldoc;
		if (xmldoc.LoadFile(filepath.c_str()) != tinyxml2::XMLError::XML_NO_ERROR)
		{
			Log("Couldnt find path");
			return;
		}

		// Root element: <GUIAnimation>
		tinyxml2::XMLElement* root = xmldoc.RootElement();
		std::string elemname = root->Name();
		if (root == nullptr)
		{
			Log("There is no root");
			return;
		}
		if (elemname != "GUIAnimation")
		{
			Log("File format corrupted");
			return;
		}


		// store xml data
		float		px;
		float		py;
		int			loop;
		int			numframes;
		std::string	texturename	= "";
		float		duration;


		// animation position: <Position>
		tinyxml2::XMLElement* elem_pos = root->FirstChildElement();
		elem_pos->FirstChildElement("x")->QueryFloatText(&px);
		elem_pos->FirstChildElement("y")->QueryFloatText(&py);
		m_vPosition = Vector2(px, py);


		// animation looping status: <Looping>
		tinyxml2::XMLElement* elem_loop = elem_pos->NextSiblingElement("Looping");
		elem_loop->QueryIntText(&loop);
		m_bLooping = (loop == 0) ? false : true;


		// # of animation frames: <GUIFrames Count = "3">
		tinyxml2::XMLElement* elem_frames = elem_loop->NextSiblingElement("GUIFrames");
		elem_frames->QueryIntAttribute("Count", &numframes);

		// animation frame: <GUIFrame>
		tinyxml2::XMLElement* elem_frame = elem_frames->FirstChildElement("GUIFrame");
		for (int i = 0; i < numframes; i++)
		{
			// texture name: <GUIFrame Name = "anim_chest_open_1">
			texturename = elem_frame->Attribute("Name");

			// duration: <Duration>
			tinyxml2::XMLElement* elem_dur = elem_frame->FirstChildElement("Duration");
			elem_dur->QueryFloatText(&duration);

			// create frame
			GUIFrame* guiframe = new GUIFrame(static_cast<GUITexture*>(hudelems[texturename]), duration);
			if (guiframe->Texture() == nullptr)
			{
				Log("GUITexture [" + texturename + "] was not loaded");
				continue;
			}

			// add frame
			AddFrame(guiframe);

			// next frame
			elem_frame = elem_frame->NextSiblingElement("GUIFrame");
		}
	}


	void GUIAnimation::AddFrame(GUIFrame * frame)
	{
		// null check
		if (frame == nullptr)
			return;

		// add frame
		m_vecFrames.push_back(frame);
	}


	void GUIAnimation::RemoveFrame(GUIFrame * frame)
	{
		// null check
		if (frame == nullptr)
			return;

		// # of frames
		size_t vecsize = m_vecFrames.size();

		// look for frame
		for (size_t i = 0; i < vecsize; ++i)
		{
			// found the frame?
			if (m_vecFrames[i] == frame)
			{
				// remove frame
				delete m_vecFrames[i];
				m_vecFrames[i] = nullptr;
				m_vecFrames.erase(m_vecFrames.begin() + i--);
				break;
			}
		}
	}


	void GUIAnimation::RemoveAllFrames(void)
	{
		// delete frames
		for (size_t i = 0; i < m_vecFrames.size(); i++)
		{
			delete m_vecFrames[i];
			m_vecFrames[i] = nullptr;
		}

		// clear vector
		m_vecFrames.clear();
	}


	void GUIAnimation::ResetAnimation(void)
	{
		// restart
		RestartAnimation();

		// pause
		PauseAnimation();
	}


	void GUIAnimation::RestartAnimation(void)
	{
		// reset time stamp data
		m_sTimeStamp.m_nCurrFrame = 0;
		m_sTimeStamp.m_fCurrDuration = 0.0f;
	}


	void GUIAnimation::PlayAnimation(void)
	{
		m_bPlaying = true;
	}


	void GUIAnimation::PauseAnimation(void)
	{
		m_bPlaying = false;
	}


	void GUIAnimation::ShowAnimation(void)
	{
		m_bShowing = true;
	}


	void GUIAnimation::HideAnimation(void)
	{
		m_bShowing = false;
	}



	//////////////////////
	// GUIAnimationMap
	//////////////////////

	GUIAnimationMap::GUIAnimationMap(void)
	{
	}


	GUIAnimationMap::~GUIAnimationMap(void)
	{
		RemoveAllAnimation();
	}


	void GUIAnimationMap::RenderAll(void)
	{
		auto iter	= m_mapAnimations.begin();
		auto end	= m_mapAnimations.end();

		for (; iter != end; iter++)
		{
			iter->second->Render();
		}
	}


	void GUIAnimationMap::UpdateAll(float dt)
	{
		auto iter	= m_mapAnimations.begin();
		auto end	= m_mapAnimations.end();

		for (; iter != end; iter++)
		{
			iter->second->Update(dt);
		}
	}


	void GUIAnimationMap::LoadGUIAnimation(std::string guianimfilename, GUIMap& hudelems)
	{
		// Create new animation
		GUIAnimation* animation = new GUIAnimation();

		// Load animation
		animation->LoadGUIAnimation(guianimfilename, hudelems);

		// Add animation to map
		AddAnimation(guianimfilename, animation);
	}


	void GUIAnimationMap::AddAnimation(std::string name, GUIAnimation* animation)
	{
		// null check
		if (animation == nullptr)
		{
			Log("Animation is null!");
			return;
		}

		// name check
		if (name == "")
		{
			Log("Animation name is not valid!");
			return;
		}

		// name conflict check
		if (m_mapAnimations.find(name) != m_mapAnimations.end())
		{
			Log("Animation [" + name + "] already exists!");
			return;
		}

		// add animation
		m_mapAnimations[name] = animation;
	}


	void GUIAnimationMap::RemoveAnimation(std::string name)
	{
		// find animation
		auto found = m_mapAnimations.find(name);

		// animation not found
		if (found == m_mapAnimations.end())
			return;

		// remove animation
		m_mapAnimations.erase(found);
	}


	void GUIAnimationMap::RemoveAllAnimation(void)
	{
		auto iter = m_mapAnimations.begin();
		auto end = m_mapAnimations.end();

		for (; iter != end; iter++)
		{
			delete iter->second;
			iter->second = nullptr;
		}
		m_mapAnimations.clear();
	}


}

