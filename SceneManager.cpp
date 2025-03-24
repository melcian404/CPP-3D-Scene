/* 
Author: Melissa Cianfarano
Original Completion: 07/20/2024
Project Objection: Create a 3D model scene using texture, cameras, light, color, and object rendering.
Add navigational components with hot keys for navigation to pan all directions as well as track mouse movements.
Project Revision: 03/23/2025
Revision Objection: Add a new complex object to the scene with appropriate textures, pathways, methods, and meshes.
Combine object methods and clean up code.
*/

/*
Author: Melissa Cianfarano
Sections edited: SceneManager(ShaderManager), SceneManager::SceneManager, 
Sections added: SceneManager::LoadSceneTextures, SceneManager::DefineObjectMaterials, ceneManager::SetupSceneLights, 
SceneManager::PrepareScene, SceneManager::RenderScene, RenderBackground, RenderRingStack, RenderCastle, RenderTrees,
RenderPyramid, RenderBook, RenderSoccer, RenderRing;
*/

///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();

	// Added texture collection 
	for (int i = 0; i < 16; i++) {
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	if (NULL != m_basicMeshes)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}

	// Destroy openGL textures
	DestroyGLTextures();
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}


		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


//  This method is for loading scene textures
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;

	// Added texture paths to jpg/png images
	bReturn = CreateGLTexture(
		"../../Utilities/textures/Tabletop.jpg", "Tabletop"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/circlestripe.jpg", "circlestripe"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/rocket.jpg", "Rocket"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/pages.jpg", "Pages"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/soccer.jpg", "Soccer"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/treetop.png", "Treetop"
	);

	bReturn = CreateGLTexture(
		"../../Utilities/textures/bark.png", "Bark"
	);

	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}

//  Define object materials
void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL glassMaterial;
	glassMaterial.ambientColor = glm::vec3(0.4f, 0.4f, 0.4f);
	glassMaterial.ambientStrength = 0.3f;
	glassMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	glassMaterial.specularColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glassMaterial.shininess = 85.0;
	glassMaterial.tag = "glass";

	m_objectMaterials.push_back(glassMaterial);

	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.ambientStrength = 0.2f;
	woodMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	woodMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.shininess = 0.3;
	woodMaterial.tag = "wood";

	m_objectMaterials.push_back(woodMaterial);

	OBJECT_MATERIAL goldMaterial;
	goldMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.1f);
	goldMaterial.ambientStrength = 0.4f;
	goldMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.2f);
	goldMaterial.specularColor = glm::vec3(0.6f, 0.5f, 0.4f);
	goldMaterial.shininess = 22.0;
	goldMaterial.tag = "gold";

	m_objectMaterials.push_back(goldMaterial);
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting - to use the default rendered 
	// lighting then comment out the following line
	//m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// Added light source positions and specs
	m_pShaderManager->setVec3Value("lightSources[0].positon", 5.0f, 2.0f, 2.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.0f, 0.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 0.929f, 0.914f, 0.345f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 1.0f, 0.922f, 0.745f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 25.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.2f);

	m_pShaderManager->setVec3Value("lightSources[1].positon", -1.0f, -2.0f, -5.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.749f, 0.749f, 0.749f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.169f, 0.404f, 1.0f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.408f, 0.545f, 0.89f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 25.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.2f);

	m_pShaderManager->setBoolValue(g_UseLightingName, true);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	// Load scene textures / Materials / Lights
	LoadSceneTextures();
	DefineObjectMaterials();
	SetupSceneLights();

	// Load shape meshes for objects 
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadPyramid4Mesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadConeMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/

void SceneManager::RenderScene()
{
	// Added shape instances to be rendered
	RenderBackground();
	RenderRingStack();
	RenderCastle();
	RenderTrees();
	RenderPyramid();
	RenderBook();
	RenderSoccer();
	RenderRing();
}

void SceneManager::RenderBackground()
{
	// declare initial variables for transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

// TABLETOP
	// set object scale
	scaleXYZ = glm::vec3(20.0f, 1.0f, 15.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Table top texture 
	SetShaderTexture("Tabletop");
	
	// draw mesh
	m_basicMeshes->DrawPlaneMesh();

// BACKGROUND WALL
	// set object scale
	scaleXYZ = glm::vec3(20.0f, 1.0f, 15.0f);

	// set object rotation
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.0f, 15.0f, -15.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set wall color
	SetShaderColor(0.004, 0.388, 0.298, 1);

	// draw mesh 
	m_basicMeshes->DrawPlaneMesh();
}

void SceneManager::RenderRingStack()
{
	// declare variables for transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

// FLATTENED BASE
	// Obect Scale
	scaleXYZ = glm::vec3(0.8f, 3.0f, 3.5f);

	// set object rotation
	XrotationDegrees = -10.0f;
	YrotationDegrees = -35.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(10.5f, 2.9f, 0.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color, shader, material
	SetShaderColor(1, 1, 1, 1);
	SetShaderTexture("circlestripe");
	SetTextureUVScale(1.8, 0.9);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawSphereMesh();

// Elongated Cylinder
	// Set object Scale 
	scaleXYZ = glm::vec3(0.6f, 7.0f, 1.0f);

	// set object rotation
	XrotationDegrees = -10.0f;
	YrotationDegrees = -35.0f;
	ZrotationDegrees = 90.0f;

	// set object position
	positionXYZ = glm::vec3(11.0f, 3.098f, 0.5f);

	// set transformations into memory to be drawn
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set the color, material
	SetShaderColor(0.988, 0.271, 0.122, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();
}

void SceneManager::RenderCastle() {

	// declare variables for transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

// FAR LEFT LOWER BOX
		// Obect Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(-9.5f, 2.53f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color, shader, material
	SetShaderColor(1, 0.9, 0, 1);
	SetShaderMaterial("glass");

	// draw the mesh
	m_basicMeshes->DrawBoxMesh();

// FAR LEFT PYRAMID
	// Set object scale
	scaleXYZ = glm::vec3(5.0f, 4.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(-9.5f, 7.0f, -11.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and material
	SetShaderColor(0, 0.1, 1, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();

// LEFT LOWER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(-4.5f, 2.53f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.1, 0.8, 0, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// LEFT UPPER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(-4.5f, 7.5f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.5, 0, 0.8, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// LEFT LARGE PYRAMID
	// Set object scale
	scaleXYZ = glm::vec3(5.0f, 8.0f, 5.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set object position
	positionXYZ = glm::vec3(-4.5f, 14.0f, -11.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(1, 0, 0, 1);
	SetShaderMaterial("glass");

	m_basicMeshes->DrawPyramid4Mesh();

// MIDDLE UPPER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(0.5f, 7.5f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(1, 0.9, 0, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// MIDDLE PYRAMID
	// Set object Scale 
	scaleXYZ = glm::vec3(5.0f, 4.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.5f, 12.0f, -11.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0, 0.1, 1, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();

// RIGHT LOWER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(5.5f, 2.53f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.1, 0.8, 0, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// RIGHT UPPER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(5.5f, 7.5f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.5, 0, 0.8, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// RIGHT LARGE PYRAMID
	// Set object scale
	scaleXYZ = glm::vec3(5.0f, 8.0f, 5.0f);

	// Set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// Set object position
	positionXYZ = glm::vec3(5.5f, 14.0f, -11.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set color and material
	SetShaderColor(1, 0, 0, 1);
	SetShaderMaterial("glass");

	// Draw mesh
	m_basicMeshes->DrawPyramid4Mesh();

// FAR RIGHT LOWER BOX
	// Set object Scale
	scaleXYZ = glm::vec3(5.0f, 5.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(10.5f, 2.53f, -11.5f);

	// set transformations into memory and draw
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(1, 0.9, 0, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// FAR RIGHT PYRAMID
	// Set object scale
	scaleXYZ = glm::vec3(5.0f, 4.0f, 5.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(10.5f, 7.0f, -11.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0, 0.1, 1, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();
}

void SceneManager::RenderTrees(){

	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

// LEFT TREE TOP
	// Set object scale
	scaleXYZ = glm::vec3(3.0f, 8.0f, 3.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(-15.5f, 2.53f, -9.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and material
	SetShaderTexture("Treetop");
	SetTextureUVScale(2.0, 4.0);
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawConeMesh();

// LEFT TREE BASE
	// Set object scale
	scaleXYZ = glm::vec3(0.5f, 3.0f, 0.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(-15.5f, 0.0f, -9.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and material
	SetShaderTexture("Bark");
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("wood");

	// Draw mesh
	m_basicMeshes->DrawCylinderMesh();

// RIGHT TREE TOP
	// Set object scale
	scaleXYZ = glm::vec3(3.0f, 8.0f, 3.0f);

	// Set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(16.5f, 2.53f, -9.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and material
	SetShaderTexture("Treetop");
	SetTextureUVScale(2.0, 4.0);
	SetShaderMaterial("wood");

	m_basicMeshes->DrawConeMesh();

// RIGHT TREE BASE
	// Set object scale
	scaleXYZ = glm::vec3(0.5f, 3.0f, 0.5f);

	// Set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position   L-R     U-D    F-B
	positionXYZ = glm::vec3(16.5f, 0.0f, -9.5f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and material
	SetShaderTexture("Bark");
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("wood");

	m_basicMeshes->DrawCylinderMesh();
}

void SceneManager::RenderPyramid()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

// FRONT AND BACK FACE
	// Obect Scale using XYZ
	scaleXYZ = glm::vec3(3.9f, 3.9f, 3.9f);

	// set Ojbect rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 10.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(-5.0f, 1.98f, 7.76f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(1, 0.11, 0.11, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();

// RIGHT FACE
	// Set object scale
	scaleXYZ = glm::vec3(3.85f, 3.85f, 3.85f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 10.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(-4.96f, 1.98f, 7.76f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(1, 0.941, 0.11, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();

// LEFT FACE
	// Set object scale
	scaleXYZ = glm::vec3(3.85f, 3.85f, 3.85f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 10.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(-5.02f, 1.98f, 7.76f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.11, 0.141, 1, 1);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawPyramid4Mesh();
}

void SceneManager::RenderBook()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// Set object scale
	scaleXYZ = glm::vec3(11.0f, 1.7f, 11.0f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.75f, 0.96f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and scale
	SetShaderTexture("Rocket");
	SetTextureUVScale(1.0, 1.0);

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// PAGES
	// Set object scale
	scaleXYZ = glm::vec3(11.0f, 1.55f, 11.02f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.78f, 0.95f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture and scale
	SetShaderTexture("Pages");
	SetTextureUVScale(1.0, 1.0);	

	// draw mesh
	m_basicMeshes->DrawBoxMesh();

// BOOK BINDING
	// Set object scale
	scaleXYZ = glm::vec3(11.0f, 1.56f, 11.01f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(0.749f, 0.95f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color
	SetShaderColor(859, 0.306, 0.306, 1);

	// draw mesh
	m_basicMeshes->DrawBoxMesh();
}

void SceneManager::RenderSoccer()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// Set object scale
	scaleXYZ = glm::vec3(2.75f, 2.75f, 2.75f);

	// set object rotation
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set object position
	positionXYZ = glm::vec3(-10.0f, 2.75f, 2.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture, scale, and material
	SetShaderTexture("Soccer");
	SetTextureUVScale(1.2, 0.8);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawSphereMesh();
}

void SceneManager::RenderRing()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// Set object scale
	scaleXYZ = glm::vec3(1.8f, 1.8f, 2.8f);

	// set object rotation
	XrotationDegrees = 310.0f; 
	YrotationDegrees = 42.2f;   
	ZrotationDegrees = 45.0f;  

	// set object position
	positionXYZ = glm::vec3(5.75f, 1.92f, 5.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set color and material
	SetShaderColor(0.78, 0.78, 0.78, 0.5);
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawTorusMesh();	
}
