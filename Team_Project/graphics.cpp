#include "graphics.h"

Graphics::Graphics()
{
	m_camera = nullptr;
	m_shader = nullptr;
	m_sun = nullptr;
	// m_sphere2 = nullptr;
	m_moon = nullptr;
	m_mesh = nullptr;
	m_skybox = nullptr;
}

Graphics::~Graphics()
{
	delete m_camera;
	delete m_shader;
	delete m_sun;
	//delete m_sphere2;
	delete m_moon;
	delete m_mesh;
	delete m_skybox;
	for (Planet& planet : m_planets) {
		delete planet.sphere;
	}
}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif

	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	if (!InitializeSkybox()) {
		printf("Skybox Failed to Initialize\n");
		return false;
	}

	// Starship
	m_mesh = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png");

	// The Sun
	m_sun = new Sphere(64, "assets\\Term Project Assets\\2k_sun.jpg");

	// add all planet
	AddPlanet("Mercury", "assets\\Term Project Assets\\Mercury.jpg", 0.138f, 5.22f, 0.60f, 1.0f, 0.0f);
	AddPlanet("Venus", "assets\\Term Project Assets\\Venus.jpg", 0.342f, 9.77f, 0.45f, 0.8f, 177.0f);
	AddPlanet("Earth", "assets\\Term Project Assets\\2k_earth_daymap.jpg", 0.360f, 13.50f, 0.30f, 1.5f, 23.5f);
	AddPlanet("Mars", "assets\\Term Project Assets\\Mars.jpg", 0.192f, 20.57f, 0.25f, 1.3f, 25.0f);
	AddPlanet("Jupiter", "assets\\Term Project Assets\\Jupiter.jpg", 4.035f, 70.25f, 0.125f, 2.2f, 3.0f);
	AddPlanet("Saturn", "assets\\Term Project Assets\\Saturn.jpg", 3.402f, 128.75f, 0.09f, 2.0f, 26.7f);
	AddPlanet("Uranus", "assets\\Term Project Assets\\Uranus.jpg", 1.443f, 259.07f, 0.065f, 1.4f, 97.8f);
	AddPlanet("Neptune", "assets\\Term Project Assets\\Neptune.jpg", 1.395f, 405.95f, 0.05f, 1.3f, 28.3f);

	// The Earth
	//m_sphere2 = new Sphere(48, "assets\\2k_earth_daymap.jpg");

	// The moon
	m_moon = new Sphere(48, "assets\\Term Project Assets\\2k_moon.jpg");

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void Graphics::HierarchicalUpdate2(double dt) {

	// Update your animation for the solar system here.
	//sun
	glm::mat4 sunModel = glm::mat4(1.0f);
	sunModel *= glm::rotate(glm::mat4(1.0f), 0.3f * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
	sunModel *= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	m_sun->Update(sunModel);

	// plant
	//glm::mat4 plantModel = glm::mat4(1.0f);
	//plantModel *= glm::rotate(glm::mat4(1.0f), 0.6f * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
	//plantModel *= glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f));
	//plantModel *= glm::rotate(glm::mat4(1.0f), 1.5f * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
	//plantModel *= glm::scale(glm::mat4(1.0f), glm::vec3(0.45f, 0.45f, 0.45f));
	//m_sphere2->Update(plantModel);
	
	// planet
	for (Planet& planet : m_planets) {
		glm::mat4 planetModel = glm::mat4(1.0f);
		planetModel *= glm::rotate(glm::mat4(1.0f), planet.orbitSpeed * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
		planetModel *= glm::translate(glm::mat4(1.0f), glm::vec3(planet.orbitRadius, 0.0f, 0.0f));
		planetModel *= glm::rotate(glm::mat4(1.0f), glm::radians(planet.axialTilt), glm::vec3(0.0f, 0.0f, 1.0f));
		planetModel *= glm::rotate(glm::mat4(1.0f), planet.rotationSpeed * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
		planetModel *= glm::scale(glm::mat4(1.0f), glm::vec3(planet.radius, planet.radius, planet.radius));
		planet.sphere->Update(planetModel);
	}

	//moon
	glm::mat4 moonModel = glm::mat4(1.0f);
	moonModel *= glm::rotate(glm::mat4(1.0f), 0.30f * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
	moonModel *= glm::translate(glm::mat4(1.0f), glm::vec3(13.00f, 0.0f, 0.0f));
	moonModel *= glm::rotate(glm::mat4(1.0f), glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	moonModel *= glm::rotate(glm::mat4(1.0f), 1.0f * (float)dt, glm::vec3(0.0f, 1.0f, 0.0f));
	moonModel *= glm::translate(glm::mat4(1.0f), glm::vec3(1.2f, 0.0f, 0.0f));
	moonModel *= glm::scale(glm::mat4(1.0f), glm::vec3(0.097f, 0.097f, 0.097f));
	m_moon->Update(moonModel);

	//starship

	//gets the starships orientation position to match with camera
	glm::vec3 shipPos = m_camera->GetStarshipPos();
	glm::vec3 shipFront = glm::normalize(m_camera->GetStarshipFront());
	glm::vec3 shipUp = glm::normalize(m_camera->GetStarshipUp());
	glm::vec3 shipRight = glm::normalize(glm::cross(shipUp, shipFront));

	//orients the starship with the camera position.
	glm::mat4 shipOrientation(1.0f);
	shipOrientation[0] = glm::vec4(shipRight, 0.0f);
	shipOrientation[1] = glm::vec4(shipUp, 0.0f);
	shipOrientation[2] = glm::vec4(shipFront, 0.0f);

	//render the starship model
	glm::mat4 shipModel = glm::mat4(1.0f);
	shipModel *= glm::translate(glm::mat4(1.0f), shipPos);
	shipModel *= shipOrientation;
	shipModel *= glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //ship is facing away from camera/sun
	shipModel *= glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
	m_mesh->Update(shipModel);

}

void Graphics::AddPlanet(const std::string& name, const char* texturePath, float radius, float orbitRadius, float orbitSpeed, float rotationSpeed, float axialTilt) {
	Planet planet;
	planet.sphere = new Sphere(48, texturePath);
	planet.name = name;
	planet.radius = radius;
	planet.orbitRadius = orbitRadius;
	planet.orbitSpeed = orbitSpeed;
	planet.rotationSpeed = rotationSpeed;
	planet.axialTilt = axialTilt;

	m_planets.push_back(planet);
}

void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist,
	std::vector<float> rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2])
	);
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed[0] * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}

void Graphics::Render()
{
	//clear the screen
	glClearColor(0.5f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the skybox
	RenderSkybox();

	// Start the correct program
	m_shader->Enable();

	// pass the value of uniform to shader
	glm::vec3 cameraPos = m_camera->GetCameraPos();
	glUniform3f(m_lightPos, 0.0f, 0.0f, 0.0f);
	glUniform3fv(m_viewPos, 1, glm::value_ptr(cameraPos));
	glUniform1f(m_ambientStrength, 0.15f);
	glUniform1f(m_specularStrength, 0.35f);
	glUniform1f(m_shininess, 32.0f);

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	// Render the objects
	/*if (m_cube != NULL){
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cube->GetModel()));
		m_cube->Render(m_positionAttrib,m_colorAttrib);
	}*/

	if (m_mesh != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));
		if (m_mesh->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_mesh->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_mesh->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	if (m_sun != NULL) {
		glUniform1i(m_isSun, true); // is Sun bool true
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sun->GetModel()));
		if (m_sun->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sun->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_sun->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	/*if (m_sphere2 != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sphere2->GetModel()));
		if (m_sphere2->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_sphere2->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_sphere2->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}*/
	glUniform1i(m_isSun, false); // is Sun bool false
	for (Planet& planet : m_planets) {
		if (planet.sphere != NULL) {
			glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(planet.sphere->GetModel()));
			if (planet.sphere->hasTex) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, planet.sphere->getTextureID());
				GLuint sampler = m_shader->GetUniformLocation("sp");
				if (sampler == INVALID_UNIFORM_LOCATION)
				{
					printf("Sampler Not found not found\n");
				}
				glUniform1i(sampler, 0);
				planet.sphere->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
			}
		}
	}


	// Render Moon
	glUniform1i(m_isSun, false); // is Sun bool false
	if (m_moon != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_moon->GetModel()));
		if (m_moon->hasTex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_moon->getTextureID());
			GLuint sampler = m_shader->GetUniformLocation("sp");
			if (sampler == INVALID_UNIFORM_LOCATION)
			{
				printf("Sampler Not found not found\n");
			}
			glUniform1i(sampler, 0);
			m_moon->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}

	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}

glm::vec3 Graphics::getPlanetPosition(const std::string& planetName) const 
{
	for (const Planet& planet : m_planets) 
	{
		if (planet.name == planetName) 
		{
			return glm::vec3(planet.sphere->GetModel()[3]); // Extract the translation component from the model matrix
		}
	}
	return glm::vec3(0.0f, 0.0f, 0.0f); // Return a default value if the planet is not found
}

bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == -1)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_colorAttrib = m_shader->GetAttribLocation("v_color");
	if (m_colorAttrib == -1)
	{
		printf("v_color attribute not found\n");
		anyProblem = false;
	}

	// Locate the texture coordinate vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_tc");
	if (m_tcAttrib == -1)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}

	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		anyProblem = false;
	}

	m_lightPos = m_shader->GetUniformLocation("lightPos");
	m_viewPos = m_shader->GetUniformLocation("viewPos");
	m_ambientStrength = m_shader->GetUniformLocation("ambientStrength");
	m_specularStrength = m_shader->GetUniformLocation("specularStrength");
	m_shininess = m_shader->GetUniformLocation("shininess");
	m_isSun = m_shader->GetUniformLocation("isSun");

	return anyProblem;
}

bool Graphics::InitializeSkybox()
{
	m_skybox = new Skybox();

	if (!m_skybox->Initialize())
	{
		return false;
	}
	return true;
}

void Graphics::RenderSkybox()
{
	if (m_skybox != nullptr)
	{
		m_skybox->Render(m_camera->GetProjection(), m_camera->GetView());
	}
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}
