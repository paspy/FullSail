#ifndef CGUIManager_h__
#define CGUIManager_h__

#include <vector>
#include <string>
#include <SimpleMath.h>
#include "GUITexture.h"
#include "GUIText.h"
#include "GUISlider.h"

typedef std::pair < std::wstring, G2D::Entity2D* > ObjectPair;
typedef std::vector<ObjectPair> GUIObjects;
typedef std::unordered_map<std::string, G2D::Entity2D*> GUIMap;

namespace G2D {

    using namespace DirectX::SimpleMath;

    class CGUIManager {
    public:
        CGUIManager() = default;
        ~CGUIManager() = default;

        bool    AddEntity( Entity2D* entity );
        bool    RemoveEntity( Entity2D* entity );
        bool    RemoveAll();

        void    UpdateAll( float _dt );
        void    RenderAll();

        // static functions
        static void LoadXMLConfig( const std::string & filename, GUIMap & guiMap );

    private:
        GUIObjects  m_tGUIEntities;
        bool        m_bWRLock = false;

        static bool SortByLayerFunc( ObjectPair const & a, ObjectPair const & b ) { return ( a.second->Layer() < b.second->Layer() ); };

    protected:
        CGUIManager( const CGUIManager& ) = delete;
        CGUIManager& operator= ( const CGUIManager& ) = delete;

    };

    namespace Factory {
        Entity2D* CreateGUITexture(
            std::string textureName,
			std::string textureFile,
            Vector2 position,
            float rotation = 0.0f,
            float scale = 1.0f,
            float layer = 0,
            Color color = { 1.f, 1.f, 1.f, 1.f },
            bool center = false,
            bool active = true
        );

        Entity2D* CreateGUIText(
            std::wstring text,
            Vector2 postion,
            float rotation = 0.0f,
            float scale = 1.0f,
            float layer = 0,
            Color color = { 1.f, 1.f, 1.f, 1.f },
            bool center = false,
            bool active = true );

        Entity2D* CreateGUISlider(
            std::string frontTexture,
			std::string frontFile,
            std::string backTexture,
			std::string backFile,
            Vector2 postion,
            float rotation = 0.0f,
            float scale = 1.0f,
            float layer = 0,
            Color color1 = { 1.f, 1.f, 1.f, 1.f },
            Color color2 = { 1.f, 1.f, 1.f, 1.f },
            bool center = false,
            bool active = true );
    }
}



#endif // CGUIManager_h__
