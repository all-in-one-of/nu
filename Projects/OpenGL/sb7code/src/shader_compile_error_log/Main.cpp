#include <sb7.h>
#include <vmath.h>
#include <string>
#include <iostream>

class spinningcube_app : public sb7::application
{
	void init()
	{
		static const char title[] = "Spinny Cube";

		sb7::application::init();

		memcpy (info.title, title, sizeof (title));		
	}

	virtual void startup ()
	{
		static const char * vs_source [] =
			{
				"#version 450 core                                           \n"
				"in vec4 position;                                           \n"
				"out VS_OUT                                                  \n"
				"{                                                           \n"
					"vec4 color;                                               \n"
				"} vs_out;                                                   \n"
				"uniform mat4 mv_matrix;                                     \n"
				"uniform mat4 proj_matrix;                                   \n"
				"void main(void)                                             \n"
				"{                                                           \n"
					"gl_Position = proj_matrix * mv_matrix * position;         \n"
					"vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0); \n"
				"}                                                           \n"
			};

		static const char * fs_source[] =
			{
/*				"#version 450 core                                           \n"
				"uniform scale; \n"
				"uniform vec3 bias; \n"
				"void main(void) \n"
				"{ \n"
					"color = vec4(1.0, 0.5, 0.2, 1.0) * scale + bias; \n"
				"} \n"
*/
				"#version 450 core                                           \n"
				"out vec4 color;                                                  \n"
				"in VS_OUT                                                   \n"
				"{                                                           \n"
					"vec4 color;                                               \n"
				"} fs_in;                                                    \n"
				"void main(void)                                             \n"
				"{                                                           \n"
					"color = fs_in.color;                                      \n"
				"}                                                           \n"				
			};

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, vs_source, NULL);
		glCompileShader(vs);

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, fs_source, NULL);
		glCompileShader(fs);

		GLint log_length;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &log_length);
		std::string str;
		str.reserve(log_length);
		glGetShaderInfoLog(fs, log_length, NULL, str.c_str());
		std::cout << "LOG: \n" << str.c_str();

		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		mv_location = glGetUniformLocation(program, "mv_matrix");
		proj_location = glGetUniformLocation(program, "proj_matrix");

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		static const GLfloat vertex_positions[] =
			{
				-0.25f,  0.25f, -0.25f,
				-0.25f, -0.25f, -0.25f,
				0.25f, -0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
				0.25f,  0.25f, -0.25f,
				-0.25f,  0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
				0.25f, -0.25f,  0.25f,
				0.25f,  0.25f, -0.25f,

				0.25f, -0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,
				0.25f,  0.25f, -0.25f,

				0.25f, -0.25f,  0.25f,
				-0.25f, -0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,

				-0.25f, -0.25f,  0.25f,
				-0.25f,  0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,

				-0.25f, -0.25f,  0.25f,
				-0.25f, -0.25f, -0.25f,
				-0.25f,  0.25f,  0.25f,

				-0.25f, -0.25f, -0.25f,
				-0.25f,  0.25f, -0.25f,
				-0.25f,  0.25f,  0.25f,

				-0.25f, -0.25f,  0.25f,
				0.25f, -0.25f,  0.25f,
				0.25f, -0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
				-0.25f, -0.25f, -0.25f,
				-0.25f, -0.25f,  0.25f,

				-0.25f,  0.25f, -0.25f,
				0.25f,  0.25f, -0.25f,
				0.25f,  0.25f,  0.25f,

				0.25f,  0.25f,  0.25f,
				-0.25f,  0.25f,  0.25f,
				-0.25f,  0.25f, -0.25f
			};

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER,
								 sizeof(vertex_positions),
								 vertex_positions,
								 GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
	}

	virtual void render(double currentTime)
	{
		static const GLfloat color[] = {0.0f, 0.25f, 0.0f, 1.0f};
		static const GLfloat one = 1.0f;

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, color);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glUseProgram(program);

		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

		float f = (float)currentTime * 0.3;
		vmath::mat4 mv_matrix =
			vmath::translate(0.0f, 0.0f, -0.4f) *
			vmath::translate(sinf(2.1f * f) * 0.5f,
											 cosf(1.7f * f) * 0.5f,
											 sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	virtual void shutdown()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteProgram(program);
		glDeleteBuffers(1, &buffer);
	}

	void onResize(int w, int h)
	{
		sb7::application::onResize(w,h);

		aspect = (float)w / (float)h;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}
	

private:
	GLuint      vao;
	GLuint      buffer;
	GLuint      program;
	GLint       mv_location;
	GLint       proj_location;

	float aspect;
	vmath::mat4 proj_matrix;
};

DECLARE_MAIN(spinningcube_app)
