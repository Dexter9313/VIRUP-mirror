#include "vr/Controller.hpp"

//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device property and turn it
//			into a std::string
//-----------------------------------------------------------------------------
std::string GetTrackedDeviceString(vr::IVRSystem* pHmd,
                                   vr::TrackedDeviceIndex_t unDevice,
                                   vr::TrackedDeviceProperty prop,
                                   vr::TrackedPropertyError* peError = nullptr)
{
	uint32_t requiredBufferLen = pHmd->GetStringTrackedDeviceProperty(
	    unDevice, prop, nullptr, 0, peError);
	if(requiredBufferLen == 0)
	{
		return "";
	}

	auto pchBuffer = new char[requiredBufferLen];
	pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer,
	                                     requiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

int getAxisId(vr::IVRSystem* vr_pointer, unsigned int deviceId, int axis)
{
	// Used to get/store property ids for the xy of the pad and the analog
	// reading of the trigger
	for(int i = 0; i < static_cast<int>(vr::k_unControllerStateAxisCount); i++)
	{
		int prop = vr_pointer->GetInt32TrackedDeviceProperty(
		    // NOLINTNEXTLINE(bugprone-misplaced-widening-cast)
		    deviceId, static_cast<vr::ETrackedDeviceProperty>(
		                  vr::Prop_Axis0Type_Int32 + i));

		if(prop == axis)
		{
			return i;
		}
	}
	return -1;
}

Controller::Controller(vr::IVRSystem* vr_pointer, unsigned int nDevice,
                       Side side)
    : side(side)
    , nDevice(nDevice)
    , vr_pointer(vr_pointer)
    , triggerid(getAxisId(vr_pointer, nDevice, vr::k_eControllerAxis_Trigger))
    , padid(getAxisId(vr_pointer, nDevice, vr::k_eControllerAxis_TrackPad))
    , shaderProgram("controllers")
{
	std::string render_model_name = GetTrackedDeviceString(
	    vr_pointer, nDevice, vr::Prop_RenderModelName_String);

	vr::RenderModel_t* model;
	vr::EVRRenderModelError error;
	while(true)
	{
		qDebug() << QString("Starting loading render model's model (")
		                + render_model_name.c_str() + ")";
		error = vr::VRRenderModels()->LoadRenderModel_Async(
		    render_model_name.c_str(), &model);
		if(error != vr::VRRenderModelError_Loading)
		{
			break;
		}

		QThread::sleep(1);
	}
	qDebug() << "Render model's model succesfully loaded";

	if(error != vr::VRRenderModelError_None)
	{
		qWarning() << QString("Unable to load render model ")
		                  + render_model_name.c_str() + ". Error code: "
		                  + vr::VRRenderModels()
		                        ->GetRenderModelErrorNameFromEnum(error);
		vr::VRRenderModels()->FreeRenderModel(model);
		return;
	}

	vr::RenderModel_TextureMap_t* rm_texture;
	while(true)
	{
		qDebug() << QString("Starting loading render model's texture (")
		                + render_model_name.c_str() + ")";
		error = vr::VRRenderModels()->LoadTexture_Async(model->diffuseTextureId,
		                                                &rm_texture);
		if(error != vr::VRRenderModelError_Loading)
		{
			break;
		}

		QThread::sleep(1);
	}
	qDebug() << "Render model's texture succesfully loaded";

	if(error != vr::VRRenderModelError_None)
	{
		qWarning() << QString("Unable to load render texture id ")
		                  + model->diffuseTextureId + " for render model "
		                  + render_model_name.c_str();
		vr::VRRenderModels()->FreeRenderModel(model);
		return;
	}

	/*
	// a single vertex in a render model
	struct RenderModel_Vertex_t
	{
	    HmdVector3_t vPosition; // position in meters in device space
	    HmdVector3_t vNormal;
	    float rfTextureCoord[2];
	};

	// A texture map for use on a render model
	struct RenderModel_TextureMap_t
	{
	    uint16_t unWidth,
	        unHeight; // width and height of the texture map in pixels
	    const uint8_t* rubTextureMapData; // Map texture data. All textures are
	                                      // RGBA with 8 bits per channel per
	                                      // pixel. Data size is width * height
	                                      // * 4ub
	};

	// Contains everything a game needs to render a single tracked or static
	// object for the user.
	struct RenderModel_t
	{
	    uint64_t ulInternalHandle;               // Used internally by SteamVR
	    const RenderModel_Vertex_t* rVertexData; // Vertex data for the mesh
	    uint32_t unVertexCount; // Number of vertices in the vertex data
	    const uint16_t*
	        rIndexData; // Indices into the vertex data for each triangle
	    uint32_t unTriangleCount; // Number of triangles in the mesh. Index
	                              // count is 3 * TriangleCount
	    RenderModel_TextureMap_t
	        diffuseTexture; // RGBA diffuse texture for the model
	};
	*/

	std::vector<float> vertices;
	std::vector<unsigned int> elements;

	for(unsigned int i(0); i < model->unVertexCount; ++i)
	{
		vertices.push_back(model->rVertexData[i].vPosition.v[0]);
		vertices.push_back(model->rVertexData[i].vPosition.v[1]);
		vertices.push_back(model->rVertexData[i].vPosition.v[2]);
		vertices.push_back(model->rVertexData[i].vNormal.v[0]);
		vertices.push_back(model->rVertexData[i].vNormal.v[1]);
		vertices.push_back(model->rVertexData[i].vNormal.v[2]);
		vertices.push_back(model->rVertexData[i].rfTextureCoord[0]);
		vertices.push_back(model->rVertexData[i].rfTextureCoord[1]);
	}

	for(unsigned int i(0); i < model->unTriangleCount * 3; ++i)
	{
		elements.push_back(model->rIndexData[i]);
	}

	mesh.setVertices(vertices, shaderProgram,
	                 {{"position", 3}, {"normal", 3}, {"texcoord", 2}},
	                 elements);

	tex = new GLTexture(
	    GLTexture::Tex2DProperties(rm_texture->unWidth, rm_texture->unHeight),
	    {}, {rm_texture->rubTextureMapData});

	shaderProgram.setUniform("alpha", 1.0f);
	if(side == Side::LEFT)
	{
		shaderProgram.setUniform("color", QVector3D(1.0f, 0.0f, 0.0f));
	}
	else
	{
		shaderProgram.setUniform("color", QVector3D(0.0f, 1.0f, 0.0f));
	}

	vr::VRRenderModels()->FreeRenderModel(model);
	vr::VRRenderModels()->FreeTexture(rm_texture);
}

void Controller::update(QMatrix4x4 const& model, unsigned int nDevice)
{
	this->nDevice = nDevice;
	this->model   = model;
	vr::VRControllerState_t controllerState;
	if(!vr_pointer->GetControllerState(nDevice, &controllerState,
	                                   sizeof(controllerState)))
	{
		return;
	}
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	triggerValue = controllerState.rAxis[triggerid].x;
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	padCoords[0] = controllerState.rAxis[padid].x;
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	padCoords[1] = controllerState.rAxis[padid].y;
}

void Controller::render() const
{
	GLHandler::setPointSize(8);
	GLHandler::setUpRender(shaderProgram, model,
	                       GLHandler::GeometricSpace::SEATEDTRACKED);
	GLHandler::useTextures({tex});
	mesh.render();
}
