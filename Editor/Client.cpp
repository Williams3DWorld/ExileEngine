#include "Client.h"
#include "stb_image.h"

void Client::FramebufferSize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;

	glViewport(0, 0, w, h);
}

bool Client::isRunning()
{
	return !glfwWindowShouldClose(window);
}

Client::Client()
{
	Create(APP_WIDTH, APP_HEIGHT, APP_TITLE, APP_MAXIMISE, APP_FULL_SCREEN, APP_SHOW_CURSOR);
}

Client::~Client()
{
	Destroy();
}

void Client::Create(int w, int h, const char* t, bool maximise, bool fullscreen, bool showCursor)
{
	width = w;
	height = h;
	title = t;

	glewExperimental = GL_TRUE;
	
	// Initialise GLFW
	if (!glfwInit())
	{
#ifdef _DEBUG
		ExCore::Logger::PrintErr("Failed to initialise GLFW!");
#endif
		exit(EXIT_FAILURE);
	}

	// Set GLFW window hints

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	if (maximise)
		glfwWindowHint(GLFW_MAXIMIZED, 1);

	// Assign width and height of screen
	if (fullscreen)
	{
		mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		width = mode->width;
		height = mode->height;
	}

	// Initialise windows
	window = glfwCreateWindow(width, height, title, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	ExLogInfo(std::to_string(width) + " " + std::to_string(height));
	if (!window)
	{
#ifdef _DEBUG
		ExCore::Logger::PrintErr("Failed to initialise GLFW window!");
#endif
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Initialise cursor
	ExCore::Cursor::Initialise(GLFW_ARROW_CURSOR);

	// GLFW Callback functions ----------------------------------------------------
	glfwSetFramebufferSizeCallback(window, FramebufferSize);
	glfwSetKeyCallback(window, KeyEvent);
	glfwSetMouseButtonCallback(window, MouseButtonEvent);
	glfwSetCursorPosCallback(window, MousePositionEvent);
	glfwSetScrollCallback(window, MouseScrollEvent);
	// ----------------------------------------------------------------------------

	// Assign cursor settings
	(!showCursor ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED) : glfwSetCursor(window, ExCore::Cursor::GetCursor()));
	
	// Load app icon
	GLFWimage icon;
	icon.pixels = stbi_load("icons/win_icon.png", &icon.width, &icon.height, 0, 4);
	glfwSetWindowIcon(window, 1, &icon);
	stbi_image_free(icon.pixels);

	// Set rendering properties
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &width, &height);

	// Initialise GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
#ifdef _DEBUG
		ExCore::Logger::PrintErr("Failed to initialise GLEW!");
#endif
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Enable v-sync
	glfwSwapInterval(1);
	
	// Get hardware information
	rdp.Assign(glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));	// Store hardware properties

#ifdef _DEBUG
	ExCore::Logger::PrintInfo(rdp.vendor);
	ExCore::Logger::PrintInfo(rdp.model);
	ExCore::Logger::PrintInfo(rdp.gl_version);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif

	// OpenGL properties
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.075f, 0.075f, 0.075f, 1.0f);
	glViewport(0, 0, width, height);


	/* -------------------------------- INITIALISE ENGINE PIPELINE -------------------------------- */
	// Initialise render master
	RenderMaster::Initialise(USE_OPENGL | USE_FORWARD);

	// Initialise world
	World::Initialise();



	// SSBO TEST!!!
	/*ssbo = std::make_unique<ShaderStorageBufferObject>(1, 2 * sizeof(float));
	float scale[2] = { 1.f, 1.f };
	ssbo->WriteData(&scale);
	float scaleX = 4.f;
	float scaleY = 8.f;
	ssbo->WriteDataRange(0, sizeof(float), &scaleX);
	ssbo->WriteDataRange(sizeof(float), sizeof(float), &scaleY);
	float s[2] = { 0.f, 0.f };
	ssbo->ReadData(&s);
	std::cout << s[0] << std::endl;*/
	// SSBO TEST!!!



	/* ----------------------------- INITIALISE DEFAULT CONTENT (TEMP) PIPELINE ----------------------------- */
	// Get reference to OpenGL shaders from current gl render object
	std::vector<std::shared_ptr<Shader>> gl_shaders = std::dynamic_pointer_cast<GLRenderMode>(RenderMaster::GetRenderPipeline()->GetRenderObject())->GetShaders();
	
	// Texture data
	ContentManager::textures.emplace_back("textures/default_a.tga", "default_albedo", TEXTURE_ALBEDO, GL_REPEAT, GL_LINEAR); // Mat ID
	//ContentManager::textures.emplace_back("textures/dot_a.tga", "default_albedo", TEXTURE_ALBEDO, GL_REPEAT, GL_LINEAR); // Mat ID

	// Material data
	std::vector<Texture> textures = { ContentManager::textures[0] };
	ContentManager::materials.emplace_back("default_mat", MAT_OPAQUE, false, textures);

	// Add default camera object
	World::map->AddActor(std::make_shared<CameraPerspective3D>(
		.1f,				// Near
		1000.f,				// Far
		45.f,				// Fov
		STATIC_CAST(float, width) / STATIC_CAST(float, height),		// Ratio
		.1f,				// Speed
		.005f,				// Smooth step
		.22f,				// Sensitivity X
		(.22f * (STATIC_CAST(float, height) / STATIC_CAST(float, width))),	// Sensitivity Y
		glm::vec3(.0f),		// Position
		SpringArm(85.f, glm::vec3(.0f))));

	// BSP data
	ContentManager::bsps[0] = std::make_shared<Plane>(gl_shaders[SHADER_DIFFUSE_FORWARD]->GetProgram(), ContentManager::textures[0], .0f, .0f, .0f, PLANE_DIR_Z, 1.f, 1.f, 0);
	//ContentManager::bsps[1] = std::make_shared<Plane>(gl_shaders[SHADER_DIFFUSE_FORWARD]->GetProgram(), ContentManager::textures[1], ContentManager::textures[0], 2.f, .0f, .0f, PLANE_DIR_Z, 1.f, 1.f, 0);

	// -------------------------------- TEMP --------------------------------
	World::map->AddActor(ContentManager::bsps[BSP_PLANE]);
	//World::map->AddActor(ContentManager::bsps[1]);
	// -------------------------------- TEMP --------------------------------


	


	// Compile dynamic actors
	LogicManager::CompileDynamics();
}

void Client::Destroy()
{
	glfwDestroyCursor(ExCore::Cursor::GetCursor());
	glfwDestroyWindow(window);
	glfwTerminate();
}

// ---------------------------------------------------------------------- CALLBACK FUNCTIONS ----------------------------------------------------------------------
void Client::KeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ExCore::KeyInput::UpdateKey(key, scancode, action, mods);
}

void Client::MouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
	ExCore::MouseInput::UpdateButton(button, action, mods);
}

void Client::MouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset)
{
	ExCore::MouseInput::UpdateScroll(xoffset, yoffset);
}

void Client::MousePositionEvent(GLFWwindow* window, double xpos, double ypos)
{
	ExCore::MouseInput::UpdatePosition(xpos, ypos);
	World::map->GetCameraObject()->UpdateMouseRotation();
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Client::Update()
{
	LogicManager::Update();
}

void Client::Render(double &delta)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	RenderMaster::Render(delta);

	Swap();
}

void Client::Swap()
{
	glfwSwapBuffers(window);
}

void Client::Run()
{
	ExCore::Timestep _f_time_step(WorldInfo::GetFramesPerSecond());
	double delta = 0;

	bool readData(false);

	// Main loop
	while (isRunning())
	{
		glfwPollEvents();

		_f_time_step.CalcLastElapsed();
		while (_f_time_step.timeElapsed())
		{
			Update();
			_f_time_step.ResetElapsed();
		}
		_f_time_step.Lock();

		Render(delta);
	}
}

std::unique_ptr<ShaderStorageBufferObject>	Client::ssbo;

int									Client::width;
int									Client::height;
const char*							Client::title;
GLFWwindow*							Client::window;
const GLFWvidmode*					Client::mode;
ExCore::RenderDevice::Properties	Client::rdp;