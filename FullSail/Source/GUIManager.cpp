#include "pch.h"
#include "GUIManager.h"
#include "Game.h"
#include "MainWindow.h"

namespace G2D {
    //CGraphics2D& CGUIManager::m_stGraphics2D = CGraphics2D::GetRef();

    bool CGUIManager::AddEntity( Entity2D* entity ) {
        FS_ASSERT( entity != nullptr, "CGUIManager::AddEntity - parameter cannot be null" );

        m_tGUIEntities.push_back( ObjectPair( entity->Name(), entity ) );

        entity->AddRef();
        std::stable_sort( m_tGUIEntities.begin(), m_tGUIEntities.end(), SortByLayerFunc );
        return true;
    }

    bool CGUIManager::RemoveEntity( Entity2D* entity ) {
        FS_ASSERT( entity != nullptr, "CGUIManager::RemoveEntity - parameter cannot be null" );
        FS_ASSERT( m_bWRLock == false, "CGUIManager::RemoveEntity - cannot remove while iterating" );

        for ( auto i = 0; i < m_tGUIEntities.size(); i++ ) {
            if ( m_tGUIEntities[i].second == entity ) {
                m_tGUIEntities.erase( m_tGUIEntities.begin() + i );
                entity->Release();
                return true;
            }
        }
        return false;
    }

    bool CGUIManager::RemoveAll() {
        FS_ASSERT( m_bWRLock == false, "CGUIManager::RemoveAll - cannot remove while iterating" );

        // lock before iterate
        m_bWRLock = true;
        {
            for ( auto &obj : m_tGUIEntities ) {
                obj.second->Release();
                obj.second = nullptr;
            }
            m_tGUIEntities.clear();
        }
        // lock after iterate
        m_bWRLock = false;
        return true;
    }

    void CGUIManager::UpdateAll( float _dt ) {
        FS_ASSERT( m_bWRLock == false, "CGUIManager::UpdateAll - cannot update while iterating" );

        // lock before iterate
        m_bWRLock = true;
        {
            for ( auto &obj : m_tGUIEntities ) {
                obj.second->Update( _dt );
            }
        }
        // lock after iterate
        m_bWRLock = false;
    }

    void CGUIManager::RenderAll() {
        FS_ASSERT( m_bWRLock == false, "CGUIManager::RenderAll - cannot render while iterating" );
        // lock before iterate
        m_bWRLock = true;
        {
            CGraphics2D::GetRef().BeginDraw();
            for ( auto &obj : m_tGUIEntities ) {
                obj.second->Render();
            }
            CGraphics2D::GetRef().EndDraw();
        }
        // lock after iterate
        m_bWRLock = false;
    }

    void CGUIManager::LoadXMLConfig( const std::string & filename, GUIMap & guiMap ) {
        tinyxml2::XMLDocument xmlDoc;
        FS_DEBUG::Assert( ( xmlDoc.LoadFile( filename.c_str() ) == tinyxml2::XML_SUCCESS ), L"LoadXMLConfig Failed." );
        tinyxml2::XMLElement * root = xmlDoc.RootElement();
        FS_DEBUG::Assert( ( std::string( root->Name() ) == std::string( "GUIConfiguration" ) ), L"File format corrupted." );

        std::string textureName, textName, fontName, sliderName, frontTexName, backTexName;
        std::wstring textField;
        std::unordered_map<std::string, Vector2> staticMap;
        std::unordered_map<std::string, Vector2> offsetMap;
        Vector2 position;
        float rotation, scale, layer, initalValue;
        Color color, color2;
        bool center, active, vertical, animated;

        tinyxml2::XMLElement* nextElem = root->FirstChildElement( "GUITexture" );
        tinyxml2::XMLElement* childElem;
        tinyxml2::XMLElement *staticChild, *dynmaicChild;
        Vector2 val;
        int elemCount;
		CGame *pGame = (CGame*)CGame::GetApplication();
		int nWindowWidth = pGame->m_window->GetWindowWidth(),
			nWindowHeight = pGame->m_window->GetWindowHeight();
        //////////////////////////////////////////////////////////////////////////
        // Read GUITexture
        elemCount = nextElem->IntAttribute( "Count" );
        childElem = nextElem->FirstChildElement( "GUITexture" );
        for ( int i = 0; i < elemCount; i++ ) {

            textureName = childElem->Attribute( "Name" );

            childElem = childElem->FirstChildElement( "Static" );

            for ( int i = 0; i < childElem->IntAttribute( "Count" ); i++ ) {
                if ( i == 0 ) {
                    staticChild = childElem->FirstChildElement();
                } /*else {
                    staticChild = childElem->NextSiblingElement();
                }*/
                staticChild->FirstChildElement( "x" )->QueryFloatText( &val.x );
                staticChild->FirstChildElement( "y" )->QueryFloatText( &val.y );
				val.x *= nWindowWidth;
				val.y *= nWindowHeight;
                staticMap[staticChild->Name()] = val;
                staticChild = staticChild->NextSiblingElement();
            }

            childElem = childElem->NextSiblingElement( "Dynamic" );
            for ( int i = 0; i < childElem->IntAttribute( "Count" ); i++ ) {
                if ( i == 0 ) {
                    dynmaicChild = childElem->FirstChildElement();
                } /*else {
                    dynmaicChild = childElem->NextSiblingElement();
                }*/
                dynmaicChild->FirstChildElement( "x" )->QueryFloatText( &val.x );
                dynmaicChild->FirstChildElement( "y" )->QueryFloatText( &val.y );
				val.x *= nWindowWidth;
				val.y *= nWindowHeight;
                offsetMap[dynmaicChild->Name()] = val;
                dynmaicChild = dynmaicChild->NextSiblingElement();
            }

            childElem->NextSiblingElement( "Position" )->FirstChildElement( "x" )->QueryFloatText( &position.x );
            childElem->NextSiblingElement( "Position" )->FirstChildElement( "y" )->QueryFloatText( &position.y );
            childElem->NextSiblingElement( "Rotation" )->QueryFloatText( &rotation );
            childElem->NextSiblingElement( "Scale" )->QueryFloatText( &scale );
            childElem->NextSiblingElement( "Layer" )->QueryFloatText( &layer );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "x" )->QueryFloatText( &color.x );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "y" )->QueryFloatText( &color.y );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "z" )->QueryFloatText( &color.z );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "w" )->QueryFloatText( &color.w );
            childElem->NextSiblingElement( "Center" )->QueryBoolText( &center );
            childElem->NextSiblingElement( "Active" )->QueryBoolText( &active );

            childElem = childElem->Parent()->ToElement();

			// Get the correct file from the name passed in
			std::string textureFile = textureName;
			textureFile += "_%dx%d";
			char stringBuffer[50];
			sprintf_s(stringBuffer, textureFile.c_str(), nWindowWidth, nWindowHeight);
			textureFile = stringBuffer;

			// Move position from ratio into pixel position
			position.x *= nWindowWidth;
			position.y *= nWindowHeight;

            guiMap[textureName] = Factory::CreateGUITexture( textureName, textureFile, position, rotation, scale, layer, color, center, active );
            static_cast<GUITexture*>( guiMap[textureName] )->StaticPostions( staticMap );
            static_cast<GUITexture*>( guiMap[textureName] )->Offsets( offsetMap );

            childElem = childElem->NextSiblingElement( "GUITexture" );
        }

        //////////////////////////////////////////////////////////////////////////
        // Read GUIText
        nextElem = root->FirstChildElement( "GUIText" );
        elemCount = nextElem->IntAttribute( "Count" );
        childElem = nextElem->FirstChildElement( "GUIText" );
        for ( int i = 0; i < elemCount; i++ ) {

            textName = childElem->Attribute( "Name" );

            childElem = childElem->FirstChildElement( "Text" );

            if ( childElem->GetText() ) {
                textField = AssetUtilities::s2ws( childElem->GetText() );
            }

            fontName = childElem->NextSiblingElement( "Font" )->GetText();
            childElem->NextSiblingElement( "Position" )->FirstChildElement( "x" )->QueryFloatText( &position.x );
            childElem->NextSiblingElement( "Position" )->FirstChildElement( "y" )->QueryFloatText( &position.y );
            childElem->NextSiblingElement( "Rotation" )->QueryFloatText( &rotation );
            childElem->NextSiblingElement( "Scale" )->QueryFloatText( &scale );
            childElem->NextSiblingElement( "Layer" )->QueryFloatText( &layer );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "x" )->QueryFloatText( &color.x );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "y" )->QueryFloatText( &color.y );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "z" )->QueryFloatText( &color.z );
            childElem->NextSiblingElement( "Color" )->FirstChildElement( "w" )->QueryFloatText( &color.w );
            childElem->NextSiblingElement( "Center" )->QueryBoolText( &center );
            childElem->NextSiblingElement( "Active" )->QueryBoolText( &active );

            childElem = childElem->Parent()->ToElement();

			// Move position from ratio into pixel position
			position.x *= nWindowWidth;
			position.y *= nWindowHeight;

			// Apply scaling to smaller windows then 1920x1080
			scale *= (nWindowWidth/1920.0f);

            guiMap[textName] = Factory::CreateGUIText( textField, position, rotation, scale, layer, color, center, active );
            static_cast<GUIText*>( guiMap[textName] )->Font( fontName );

            childElem = childElem->NextSiblingElement( "GUIText" );
        }

        //////////////////////////////////////////////////////////////////////////
        // Read GUISlider
        nextElem = root->FirstChildElement( "GUISlider" );
        elemCount = nextElem->IntAttribute( "Count" );
        childElem = nextElem->FirstChildElement( "GUISlider" );
        for ( int i = 0; i < elemCount; i++ ) {

            sliderName = childElem->Attribute( "Name" );

            childElem = childElem->FirstChildElement( "FrontTexture" );

            frontTexName = childElem->GetText();
            backTexName = childElem->NextSiblingElement( "BackTexture" )->GetText();
            childElem->NextSiblingElement( "InitValue" )->QueryFloatText( &initalValue );
            childElem->NextSiblingElement( "Position" )->FirstChildElement( "x" )->QueryFloatText( &position.x );
            childElem->NextSiblingElement( "Position" )->FirstChildElement( "y" )->QueryFloatText( &position.y );
            childElem->NextSiblingElement( "Rotation" )->QueryFloatText( &rotation );
            childElem->NextSiblingElement( "Scale" )->QueryFloatText( &scale );
            childElem->NextSiblingElement( "Layer" )->QueryFloatText( &layer );
            childElem->NextSiblingElement( "FrontColor" )->FirstChildElement( "x" )->QueryFloatText( &color.x );
            childElem->NextSiblingElement( "FrontColor" )->FirstChildElement( "y" )->QueryFloatText( &color.y );
            childElem->NextSiblingElement( "FrontColor" )->FirstChildElement( "z" )->QueryFloatText( &color.z );
            childElem->NextSiblingElement( "FrontColor" )->FirstChildElement( "w" )->QueryFloatText( &color.w );
            childElem->NextSiblingElement( "BackColor" )->FirstChildElement( "x" )->QueryFloatText( &color2.x );
            childElem->NextSiblingElement( "BackColor" )->FirstChildElement( "y" )->QueryFloatText( &color2.y );
            childElem->NextSiblingElement( "BackColor" )->FirstChildElement( "z" )->QueryFloatText( &color2.z );
            childElem->NextSiblingElement( "BackColor" )->FirstChildElement( "w" )->QueryFloatText( &color2.w );
            childElem->NextSiblingElement( "Center" )->QueryBoolText( &center );
            childElem->NextSiblingElement( "Active" )->QueryBoolText( &active );
            childElem->NextSiblingElement( "Vertical" )->QueryBoolText( &vertical );
            childElem->NextSiblingElement( "Animated" )->QueryBoolText( &animated );

            childElem = childElem->Parent()->ToElement();

			// Get the correct file from the name passed in
			std::string frontTextureFile = frontTexName;
			frontTextureFile += "_%dx%d";
			char frontStringBuffer[50];
			sprintf_s(frontStringBuffer, frontTextureFile.c_str(), nWindowWidth, nWindowHeight);
			frontTextureFile = frontStringBuffer;

			std::string backTextureFile = backTexName;
			backTextureFile += "_%dx%d";
			char backStringBuffer[50];
			sprintf_s(backStringBuffer, backTextureFile.c_str(), nWindowWidth, nWindowHeight);
			backTextureFile = backStringBuffer;

			// Move position from ratio into pixel position
			position.x *= nWindowWidth;
			position.y *= nWindowHeight;

            guiMap[sliderName] = Factory::CreateGUISlider( frontTexName, frontTextureFile, backTexName, backTextureFile, position, rotation, scale, layer, color, color2, center, active );
            static_cast<GUISlider*>( guiMap[sliderName] )->Value( initalValue );
            static_cast<GUISlider*>( guiMap[sliderName] )->Vertical( vertical );
            static_cast<GUISlider*>( guiMap[sliderName] )->Animated( animated );

            childElem = childElem->NextSiblingElement( "GUISlider" );
        }

        xmlDoc.Clear();
    }

    namespace Factory {


        Entity2D* CreateGUITexture(
            std::string textureName,
			std::string textureFile,
            Vector2 position,
            float rotation /*= 0.0f*/,
            float scale /*= 1.0f*/,
            float layer /*= 0 */,
            Color color /*= {1.f, 1.f, 1.f, 1.f}*/,
            bool center /*= false*/,
            bool active/*= true*/
        ) {
			GUITexture* guiTex = newx GUITexture( textureName, textureFile );

            guiTex->Position( position );
            guiTex->Scale( scale );
            guiTex->Rotation( rotation );
            guiTex->Layer( layer );
            guiTex->Color( color );
            guiTex->CenterPrivot( center );
            guiTex->Active( active );
            return guiTex;
        }

        Entity2D * CreateGUIText(
            std::wstring text,
            Vector2 postion,
            float rotation,
            float scale,
            float layer,
            Color color,
            bool center,
            bool active
        ) {
            GUIText* guiText = newx GUIText( text, color );
            guiText->Position( postion );
            guiText->Scale( scale );
            guiText->Rotation( rotation );
            guiText->Layer( layer );
            guiText->CenterPrivot( center );
            guiText->Active( active );
            guiText->Font( "Forte" );
            return guiText;
        }

        Entity2D* CreateGUISlider(
			std::string frontTexture,
			std::string frontFile,
			std::string backTexture,
			std::string backFile,
            Vector2 postion,
            float rotation /*= 0.0f*/,
            float scale /*= 1.0f*/,
            float layer /*= 0*/,
            Color color1 /*= { 1.f, 1.f, 1.f, 1.f }*/,
            Color color2 /*= { 1.f, 1.f, 1.f, 1.f }*/,
            bool center /*= false*/,
            bool active /*= true */) {

            GUISlider* guiSlider = newx GUISlider( frontTexture, frontFile, backTexture, backFile );
            guiSlider->Position( postion );
            guiSlider->Scale( scale );
            guiSlider->Rotation( rotation );
            guiSlider->Layer( layer );
            guiSlider->CenterPrivot( center );
            guiSlider->Active( active );
            guiSlider->FrontColor( color1 );
            guiSlider->BackColor( color2 );
            return guiSlider;
        }


    }

}


