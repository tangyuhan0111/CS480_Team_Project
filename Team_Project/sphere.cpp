#include "sphere.h"

Sphere::Sphere()
{
    init(48);
    setupVertices();
    setupBuffers();
    m_innerAsteroidVBO = 0;
    m_outerAsteroidVBO = 0;
    //setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
}

Sphere::Sphere(int prec) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    //setupModelMatrix(glm::vec3(0., 0., 0.), 0., 1.);
    hasTex = false;
    m_innerAsteroidVBO = 0;
    m_outerAsteroidVBO = 0;
}

Sphere::Sphere(int prec, const char* fname) { // prec is precision, or number of slices

    init(prec);
    setupVertices();
    setupBuffers();
    setupModelMatrix(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f);

        // load texture from file
    m_texture = new Texture(fname);
    if (m_texture) 
    {
        hasTex = true;
    }
        
    else 
    {
        hasTex = false;
    }
       
    m_innerAsteroidVBO = 0;
    m_outerAsteroidVBO = 0;
}


void Sphere::Render(GLint positionAttribLoc, GLint colorAttribLoc)
{
    //glBindVertexArray(vao);
    // Enable Vertext Attributes
    glEnableVertexAttribArray(positionAttribLoc);
    glEnableVertexAttribArray(colorAttribLoc);

    // Bind your VBO buffer(s) and then setup vertex attribute pointers
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glVertexAttribPointer(positionAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colorAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
 

    // Bind your index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render
    glDrawArrays(GL_TRIANGLES, 0, getNumIndices());

    // Disable Vertex Attribuates
    glDisableVertexAttribArray(positionAttribLoc);
    glDisableVertexAttribArray(colorAttribLoc);
}

void Sphere::Render(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc)
{
    glBindVertexArray(vao);
    // Enable vertex attibute arrays for each vertex attrib
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(colAttribLoc);
    glEnableVertexAttribArray(tcAttribLoc);

    // Bind your VBO
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    // If has texture, set up texture unit(s): update here for texture rendering
    if (m_texture != NULL) {
        glUniform1i(hasTextureLoc, true);
        m_texture->bind(GL_TEXTURE0);
    }
    else
        glUniform1i(hasTextureLoc, false);


    // Bind your Element Array
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

    // Disable vertex arrays
    glDisableVertexAttribArray(posAttribLoc);
    glDisableVertexAttribArray(colAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Sphere::setupVertices() {
    std::vector<int> ind = getIndices();
    std::vector<glm::vec3> vert = getVertices();
    std::vector<glm::vec2> tex = getTexCoords();
    std::vector<glm::vec3> norm = getNormals();


    int numIndices = getNumIndices();
    for (int i = 0; i < numIndices; i++) {
        int index = ind[i];
        Vertices.push_back(Vertex(vert[index], norm[index], tex[index]));
        Indices.push_back(i);
    }
}


void Sphere::setupBuffers() {
    // For OpenGL 3
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

void Sphere::setupModelMatrix(glm::vec3 pivot, float angle, float scale) {
    pivotLocation = pivot;
    model = glm::translate(glm::mat4(1.0f), pivotLocation);
    model *= glm::rotate(glm::mat4(1.f), angle, glm::vec3(0., 1., 0));
    model *= glm::scale(glm::vec3(scale, scale, scale));
}

void Sphere::SetupInstancedBuffers(GLuint& instanceVBO, const std::vector<glm::mat4>& instanceMatrices) {
	glBindVertexArray(vao);

    if (instanceVBO == 0)
    {
        glGenBuffers(1, &instanceVBO);
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data(), GL_STATIC_DRAW);

	std::size_t vec4Size = sizeof(glm::vec4);

	glEnableVertexAttribArray(3); // Assuming location 3 is for the instance matrix
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

	glEnableVertexAttribArray(4); // Assuming location 4 is for the instance matrix
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vec4Size));

	glEnableVertexAttribArray(5); // Assuming location 5 is for the instance matrix
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * vec4Size));

    glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * vec4Size));

	glVertexAttribDivisor(3, 1); // Update per instance
	glVertexAttribDivisor(4, 1); // Update per instance
	glVertexAttribDivisor(5, 1); // Update per instance
	glVertexAttribDivisor(6, 1); // Update per instance

	glBindVertexArray(0);
}

void Sphere::RenderInstanced(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTexLoc, GLuint instanceVBO, GLsizei instanceCount) {
    glBindVertexArray(vao);

    // Enable vertex attribute arrays for each vertex attrib
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(colAttribLoc);
    glEnableVertexAttribArray(tcAttribLoc);

    // Bind your VBO
    glBindBuffer(GL_ARRAY_BUFFER, VB);

    // Set vertex attribute pointers to the load correct data. Update here to load the correct attributes.
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(colAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(tcAttribLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

    // If has texture, set up texture unit(s): update here for texture rendering
    if (m_texture != NULL) {
        glUniform1i(hasTexLoc, true);
        m_texture->bind(GL_TEXTURE0);
    }
    else 
    {
        glUniform1i(hasTexLoc, false);
    }
        
    // Bind your Element Array
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    std::size_t vec4Size = sizeof(glm::vec4);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(vec4Size));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * vec4Size));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

    // Render instanced
    glDrawElementsInstanced(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0, instanceCount);

    // Disable vertex arrays
    glDisableVertexAttribArray(posAttribLoc);
    glDisableVertexAttribArray(colAttribLoc);
    glDisableVertexAttribArray(tcAttribLoc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sphere::SetupInnerInstances(const std::vector<glm::mat4>& matrices) {
    SetupInstancedBuffers(m_innerAsteroidVBO, matrices);
}

void Sphere::SetupOuterInstances(const std::vector<glm::mat4>& matrices) {
    SetupInstancedBuffers(m_outerAsteroidVBO, matrices);
}

void Sphere::RenderInnerInstanced(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLsizei instanceCount)
{
    RenderInstanced(posAttribLoc, colAttribLoc, tcAttribLoc, hasTextureLoc, m_innerAsteroidVBO, instanceCount);
}

void Sphere::RenderOuterInstanced(GLint posAttribLoc, GLint colAttribLoc, GLint tcAttribLoc, GLint hasTextureLoc, GLsizei instanceCount)
{
    RenderInstanced(posAttribLoc, colAttribLoc, tcAttribLoc, hasTextureLoc, m_outerAsteroidVBO, instanceCount);
}

void Sphere::Update(glm::mat4 matModel) {

    model = matModel;

}

float Sphere::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

void Sphere::init(int prec) {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

    for (int i = 0; i <= prec; i++) {
        for (int j = 0; j <= prec; j++) {
            float y = (float)cos(toRadians(180.f - i * 180.f / prec));
            float x = -(float)cos(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            float z = (float)sin(toRadians(j * 360.f / prec)) * (float)abs(cos(asin(y)));
            vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
            texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
            normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
        }
    }

    // calculate triangles indices
    for (int i = 0; i < prec; i++) {
        for (int j = 0; j < prec; j++) {
            indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
            indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
            indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
            indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
        }
    }
}

// accessors
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
std::vector<int> Sphere::getIndices() { return indices; }
std::vector<glm::vec3> Sphere::getVertices() { return vertices; }
std::vector<glm::vec2> Sphere::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Sphere::getNormals() { return normals; }