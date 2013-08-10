#include "utils.hpp"

void utils::ExitOnGLError(const char* error_message)
{
    const GLenum ErrorValue = glGetError();

	if (ErrorValue != GL_NO_ERROR)
	{
		const char* APPEND_DETAIL_STRING = ": %s\n";
		const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
		const size_t message_length = strlen(error_message);
		char* display_message = (char*)malloc(message_length + APPEND_LENGTH);

		memcpy(display_message, error_message, message_length);
		memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

		fprintf(stderr, display_message, gluErrorString (ErrorValue));

		free(display_message);
		exit(EXIT_FAILURE);
	}

}

GLuint utils::LoadShader(const char* filename, GLenum shader_type)
{
	GLuint shader_id = 0;
	FILE* file;
	long file_size = -1;
	char* glsl_source;

	if (NULL != (file = fopen(filename, "rb")) &&
		0 == fseek(file, 0, SEEK_END) &&
		-1 != (file_size = ftell(file)))
	{
		rewind(file);

		if (NULL != (glsl_source = (char*)malloc(file_size + 1)))
		{
			if (file_size == (long)fread(glsl_source, sizeof(char), file_size, file))
			{
				glsl_source[file_size] = '\0';

				if (0 != (shader_id = glCreateShader(shader_type)))
				{
					glShaderSource(shader_id, 1, (const char**)&glsl_source, NULL);
					glCompileShader(shader_id);
					ExitOnGLError("Could not compile a shader");
				}
				else
					fprintf(stderr, "ERROR: Could not create a shader.\n");
			}
			else
				fprintf(stderr, "ERROR: Could not read file %s\n", filename);

			free(glsl_source);
		}
		else
			fprintf(stderr, "ERROR: Could not allocate %li bytes.\n", file_size);

		fclose(file);
	}
	else
		fprintf(stderr, "ERROR: Could not open file %s\n", filename);

	return shader_id;
}

void utils::CreateTextures() {

 //FreeImage_Initialise();

 bitmap = FreeImage_Load(FIF_JPEG,"Textures/wall.jpg", JPEG_DEFAULT);
 //bitmap = FreeImage_ConvertTo24Bits(bitmap);
 GLubyte* pixels = (GLubyte*)FreeImage_GetBits(bitmap);

 if (!bitmap)
   {
       cerr<< "image not loaded"<< endl;
   }

 bitmap_width = FreeImage_GetWidth(bitmap);
 bitmap_height = FreeImage_GetHeight(bitmap);


 //Generate Textures
 glActiveTexture(GL_TEXTURE0);
 glGenTextures(1, &TextureIds);
 glBindTexture(GL_TEXTURE_2D, TextureIds);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap_width, bitmap_height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
 glGenerateMipmap(GL_TEXTURE_2D);

 FreeImage_Unload(bitmap);

 glBindTexture(GL_TEXTURE_2D, 0);

}

GLuint utils::CreateShaders(const char* fragment, const char* vertex) {

    //load and create shaders

    ShaderIds[1] = LoadShader(fragment, GL_FRAGMENT_SHADER);
    ShaderIds[2] = LoadShader(vertex, GL_VERTEX_SHADER);

    ShaderIds[0] = glCreateProgram();
    ExitOnGLError("ERROR: Could not create the shader program");
    glAttachShader(ShaderIds[0], ShaderIds[1]);
    glAttachShader(ShaderIds[0], ShaderIds[2]);

    glLinkProgram(ShaderIds[0]);
    ExitOnGLError("ERROR: Could not link the shader program");

    return ShaderIds[0];
}

void utils::DeleteTextures() {

 glBindTexture(GL_TEXTURE_2D, 0);
 glDeleteTextures(1, &TextureIds);
 ExitOnGLError("ERROR: Could not delete textures");

}

void utils::DeleteShaders() {

 glDetachShader(ShaderIds[0], ShaderIds[1]);
 glDetachShader(ShaderIds[0], ShaderIds[2]);
 glDeleteShader(ShaderIds[1]);
 glDeleteShader(ShaderIds[2]);
 glDeleteProgram(ShaderIds[0]);
 ExitOnGLError("ERROR: Could not delete ShaderIds");

}

void utils::SetUpMatrices() {

 position = glm::vec3 (512.0f, 512.0f, 509.0f);

 direction = glm::vec3 (0.0f, 0.0f, 4.0f);

 up = glm::vec3 (0.0f, 1.0f, 0.0f);

 jump = glm::vec3 (0.0f, 1.0f, 0.0f);

 FOV = 65.0f;

 speed = 2.0f;
 mouseSpeed = 0.8f;
 horizontalAngle = 0.0f;
 verticalAngle = 0.0f;

 Translate = glm::translate(Translate, glm::vec3(3.0f,0.0f,0.0f)); // glm::mat4(1.0f)
 Rotate    = glm::mat4(1.0f); // glm::rotate(Rotate, (float)30, glm::vec3(1, 1, 0))
 Scale     = glm::mat4(1.0f); //glm::scale(Scale,glm::vec3(1,1,1))===glm::gtx::scale(2,2,2);

 matrix.Model = glm::mat4(1.0f);
 //Model[0] = glm::mat4(1.0f);
 //Model[1] = Translate * Rotate * Scale * Model[0];
 //Model[2] = Translate * Rotate * Scale * Model[1];
}

void utils::SetModelViewProjection() {

 matrix.Projection = glm::perspective(FOV, 4.0f/3.0f, 0.1f, 1500.0f);
 matrix.View = glm::lookAt(
        position,
        position+direction,
        up);
}

void utils::SetMVPuniformLocation(GLuint shader) {

 uniform.Proj = glGetUniformLocation(shader, "Projection");
 uniform.View = glGetUniformLocation(shader, "View");
 uniform.Model = glGetUniformLocation(shader, "Model");

}

void utils::RealTimeImput(sf::Window &window, short int width, short int height) {

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        //move forward
        position += direction * deltatime * speed;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        //move backward
        position -= direction * deltatime * speed;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        //strafe_left
        position -= right * deltatime * speed;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        //strafe_right
        position += right * deltatime * speed;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        //crouch
        position -= jump * deltatime * speed;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        //jump
        position += jump * deltatime * speed;
    }

    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
    sf::Mouse::setPosition(sf::Vector2i(width/2, height/2), window);
    horizontalAngle += mouseSpeed * deltatime * (float(width) / 2.0f - float(localPosition.x));
    verticalAngle   += mouseSpeed * deltatime * (float(height) / 2.0f - float(localPosition.y));

    // clamp rotation angle between 0 - 2*PI
    if (horizontalAngle > 3.14f * 2.0f) horizontalAngle = 0.01f;
    if (horizontalAngle < 0.01f) horizontalAngle = 3.14f * 2.0f;

    // clamp camera up/down values so we can't go upside down
    if (verticalAngle >= 3.14f / 2.0f) verticalAngle = 3.14f / 2.0f;
    if (verticalAngle <= -3.14f / 2.0f) verticalAngle = -3.14f / 2.0f;

    direction = glm::vec3 (
       cosf(verticalAngle) * sinf(horizontalAngle),
       sinf(verticalAngle),
       cosf(verticalAngle) * cosf(horizontalAngle)
    );

    right = glm::vec3 (
       sinf(horizontalAngle - 3.14f / 2.0f),
       0.0f,
       cosf(horizontalAngle - 3.14f / 2.0f)
    );

    up = glm::cross( right, direction );

}

void utils::Framerate() {

 elapsed = clock.restart();
 deltatime = elapsed.asSeconds();
 sumtime += elapsed.asSeconds();
 frame++;
 if(sumtime >= 1.0f)
 {
     stringstream convert;
     convert << sumtime/frame*1000;
     Mpf = convert.str();

     convert.str(string());
     convert.clear();
     convert << frame;
     Fps = convert.str();
     sumtime = 0.0f;
     frame = 0;
 }

}

