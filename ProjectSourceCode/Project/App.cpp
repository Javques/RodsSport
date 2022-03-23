
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "GLShader.hpp"
#include "Scene.hpp"
#include "Model.h"
#include "Rod.h"
#include "Sphere.h"

#include <iostream>

#include <corecrt_math_defines.h>
using namespace glm;
#include <vector>
#include <cmath>
#include <list>
#include <unordered_set>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleKeyHold(float deltaTime);

void draw(GLsizei size);
vec3 getIterator(vec3 beg, vec3 end,int nbOfPoint=10) {
    vec3 result = end - beg;
    float factor = 1.f/(nbOfPoint - 1);
    vec3 it = result * factor;
    return it;
}
// settings
unsigned int scr_width = 1024;
unsigned int scr_height = 768;
int currentModel = 1;

// camera
Camera camera(glm::vec3(0, 5, 32));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
glm::vec3 cameraPositions[3] = {  glm::vec3(0, 5, 32), glm::vec3(0, 5, 32),glm::vec3(0, 5, 32) };
glm::vec3 objectPositions[3] = {  glm::vec3(0, 3, 22), glm::vec3(0, 3, 32), glm::vec3(0, 3, 32) };

// lighting
struct PointLight pointLight;

//current model
mat4* mat4Ptr = nullptr;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int planeVAO = 0;
int drawMode = GL_TRIANGLES;

void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // positions          // normals           // texture coords
               -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
               -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

               -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
               -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

               -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
               -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
               -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
               -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

               -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
               -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

               -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
               -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
               -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(drawMode, 0, 36);
    glBindVertexArray(0);
}
void renderWall() {
    if (planeVAO == 0) {
        float planeVertices[] = {
            // positions            // normals         // texcoords
             50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

             50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
             50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  50.0f, 50.0f
        };
        // plane VAO
        unsigned int planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(planeVAO);
    glDrawArrays(drawMode, 0, 6);
    glBindVertexArray(0);

}
// renders the 3D scene
// --------------------





// buttons
enum Inputs {
    RIGHT_CLICK,
    LEFT_CLICK,
    MIDDLE_CLICK,
    SHIFT,
    W,
    A,
    S,
    D,
    KEY_9,
    KEY_0
};

int inputState[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };




//Alexis
cubeList cube(false);
float angles = 30.0f;

unsigned int line_VAO, line_VBO, ballVbo;


bool cullFace = true;
bool shadow = true;
bool texture = true;
int currentScenario = 1;
int yPos = 30;

int main()
{
 
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "OpenGL Test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // GLFW event handling setup
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // init GLEW, enable depth testing, compile and load shaders
    GLenum err = glewInit();

    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_RESCALE_NORMAL);
  
    Shader flatShader("flat.vert", "flat.frag");
    

    vec3 lightPos = vec3(0, 30, 0);

    
    GLuint unitLines = lines();
    GLuint xGrid = Xgrid();
    GLuint zGrid = Zgrid();
    unsigned int lightCubeVAO = cube.createOriginalWall();
    unsigned int sphereVAO = cube.createSphere();

    vec3 horizontalIt = vec3(1, 0, 0);
    vec3 verticalIt = vec3(0, 1, 0);


    //scenario 1
    const int gridSize = 5;
    Not nots[gridSize][gridSize];
    vector<Rod> allRods;
    //initialize grid of nots
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if ((i == 0 || j == 0)||(i== gridSize -1||j== gridSize-1)) {
                nots[i][j].setClamp(true);
            }
            else
            {
                nots[i][j].setClamp(false);
            }

            nots[i][j].setPosition(vec3((i * 10)-i, (j * 10)-j, 0));
            nots[i][j].setVectorRef(&allRods);
        }
    }

    //connect each not with rods
    int index = -1;
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            
            //normally add right + top
            //check if can add to right and to top
            if (i < gridSize - 1 && j < gridSize - 1) {

                
                Rod rightRod = Rod(nots[i][j].getPosition(), nots[i+1][j].getPosition(), horizontalIt);
                index++;
                rightRod.setOffSet();
                allRods.push_back(rightRod);
                
                nots[i][j].setRight(index);
                nots[i + 1][j].setLeft(index);

                Rod topRod = Rod(nots[i][j].getPosition(), nots[i][j+1].getPosition(), verticalIt);
                index++;     
                topRod.setOffSet();
                allRods.push_back(topRod);
                nots[i][j].setUp(index);
                nots[i][j + 1].setDown(index);
                //debug purposes
                /*if (i == 1 && j == 1) {
                    cout << nots[i][j].leftRod << "\n";
                    cout << nots[i][j].rightRod << "\n";;
                    cout << nots[i][j].upRod << "\n";
                    cout << nots[i][j].downRod << "\n";;

                }*/
                //add rod to right and top not
            }
            //add last horizontal links
            if (j == gridSize -1&&i< gridSize-1) {
                Rod rightRod = Rod(nots[i][j].getPosition(), nots[i + 1][j].getPosition(), horizontalIt);
                rightRod.setOffSet();
                allRods.push_back(rightRod);
                
                index++;
                nots[i][j].setRight(index);
                nots[i + 1][j].setLeft(index);
                
            }
            //add last vertical links
            if (i == gridSize-1 && j< gridSize-1) {
                Rod topRod = Rod(nots[i][j].getPosition(), nots[i][j + 1].getPosition(), verticalIt);

                topRod.setOffSet();
                allRods.push_back(topRod);
                

                nots[i][j + 1].setDown(index);
                nots[i][j].setUp(index);
                
            }
            
        }
    }
    //set offsets
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            nots[i][j].setOffSet();
        }
    }
    float y0 = 0;
    float x0 = 0;
    //debug purposes
    /*for (unsigned int i = 0; i < allRods.size(); i++)
    {
        std::cout << &allRods[i]<<"\n";
    }*/
    vec3 curr = nots[2][2].getPosition();


    Sphere sphere(10.f, vec3(curr.x,curr.y,-18.f), 100.f,vec3(0,0, 0.1f),vec3(0));
   

    //check if point is hit if length(point,centerSpher)<radius
    //if point is hit displace by radius/length(point,centerSphere)*unit vector(point,centerSphere)


    //setting up scenario 2
        Rod rod(vec3(0, 0, 0), vec3(9, 0, 0), vec3(1, 0, 0));
        //set offset for bending problems
        rod.setOffSet();
        rod.setCurrentPoint(4, rod.GetPresetPoint(4) + vec3(0, 0.5, 0));

        
    //scenario 3
        //sphere
        Sphere sphere2(6.f, vec3(0, yPos, 9), 100.f, vec3(0, -0.1f, 0), vec3(0));
        vector<Rod> allRodsBasket;
        float rodMass = 500.f;
        int divider = 1 / 10;
        const int nbLvls = 4;
        const int nbNotLvl = 8;
        Not basketNots[nbLvls][nbNotLvl]; 
        int height = 8;
        float ratio = 1.5;
        int offset = 20;
        //creating the net of nots for basketball
        for (int i = 0; i < nbLvls; i++) {

            for (int j = 0; j < nbNotLvl; j++) {
                if (i == 3) ratio = 0;
                if (i == 1) ratio = 3;
                if (i == 2) ratio = 2;
                basketNots[i][j].setMass(rodMass);
                basketNots[i][j].setVectorRef(&allRodsBasket);
                if (i > 0&&i<nbLvls-1) {
                    basketNots[i][j].setClamp(false);
                }
                else {
                    basketNots[i][j].setClamp(true);
                }
                switch (j) {
                case 0:
                    basketNots[i][j].setPosition(vec3(0, offset-i*height, 0+(i*ratio)));
                    break;

                case 1:
                    basketNots[i][j].setPosition(vec3(6- (i * ratio), offset -i * height,3+ (i * ratio)));
                    break;
                case 2:
                    basketNots[i][j].setPosition(vec3(9- (i * ratio), offset -i * height, 9));
                    break;
                case 3:
                    basketNots[i][j].setPosition(vec3(6- (i * ratio), offset -i * height, 15- (i * ratio)));
                    break;
                case 4:
                    basketNots[i][j].setPosition(vec3(0, offset -i * height, 18- (i * ratio)));
                    break;
                case 5:
                    basketNots[i][j].setPosition(vec3(-6+ (i * ratio), offset -i * height, 15- (i * ratio)));
                    break;
                case 6:
                    basketNots[i][j].setPosition(vec3(-9+ (i * ratio), offset -i * height, 9));
                    break;
                case 7:
                    basketNots[i][j].setPosition(vec3(-6+ (i * ratio), offset -i * height, 3+(i*ratio)));
                    break;

                }
            }
        }
        //creating the rods
        index = -1;
        std::unordered_set<int> dontDraw;
        for (int i = 0; i < nbLvls; i++) {

            for (int j = 0; j < nbNotLvl; j++) {
                if (j < nbNotLvl - 1) {
                    vec3 theIt = getIterator(basketNots[i][j].getPosition(), basketNots[i][j + 1].getPosition(),(i>0&&i<nbNotLvl-1)?5:10);
                    Rod rightRod(basketNots[i][j].getPosition(), basketNots[i][j + 1].getPosition(), theIt, (i > 0 && i < nbNotLvl - 1) ? 5 : 10,rodMass);
                    rightRod.setOffSet();
                    allRodsBasket.push_back(rightRod);
                    index++;
                    basketNots[i][j].setRight(index);
                    basketNots[i][j + 1].setLeft(index);
                    if (i > nbLvls - 2) {
                        dontDraw.insert(index);
                    }
                }
                else {
                    vec3 theIt = getIterator(basketNots[i][j].getPosition(), basketNots[i][0].getPosition(), (i > 0 && i < nbNotLvl - 1) ? 5 : 10);
                    Rod rightRod(basketNots[i][j].getPosition(), basketNots[i][0].getPosition(), theIt, (i > 0 && i < nbNotLvl - 1) ? 5 : 10, rodMass);
                    rightRod.setOffSet();
                    allRodsBasket.push_back(rightRod);
                    index++;
                    basketNots[i][j].setRight(index);
                    basketNots[i][0].setLeft(index);
                    if (i > nbLvls - 2) {
                        dontDraw.insert(index);
                    }
                }
                if (i < nbLvls - 1) {
                    vec3 theIt = getIterator(basketNots[i][j].getPosition(), basketNots[i+1][j].getPosition(), (i > 0 && i < nbNotLvl - 1) ? 10 : 10);
                    Rod topRod(basketNots[i][j].getPosition(), basketNots[i+1][j].getPosition(), theIt, (i > 0 && i < nbNotLvl - 1) ? 10 : 10, rodMass);
                    topRod.setOffSet();
                    allRodsBasket.push_back(topRod);
                    index++;
                    basketNots[i+1][j].setDown(index);
                    basketNots[i][j].setUp(index);
                    if (i == nbLvls - 2) {
                        dontDraw.insert(index);
                    }
                }
            }
        }
        //setting offsets
        for (int i = 0; i < nbLvls; i++) {
            for (int j = 0; j < nbNotLvl; j++) {
               basketNots[i][j].setOffSet();
              // cout<<basketNots[i][j].GetTotalForce().x<<","<< basketNots[i][j].GetTotalForce().y <<basketNots[i][j].GetTotalForce().z <<"\n";
            }
        }

       
        
 
    while (!glfwWindowShouldClose(window))
    {

        
        //SCENE LIGHTING SET UP

        

        // use frame time deltas rather than frames
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        handleKeyHold(deltaTime);
        
        
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.perspective_zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
        glm::mat4 view = camera.view();
       
        
        
        
        flatShader.use();
        flatShader.setMat4("projection", projection);
        flatShader.setMat4("view", view);


        // other draw calls
        glBindVertexArray(0);
     
        


        glBindVertexArray(xGrid);
        drawXGrid(flatShader);
        glBindVertexArray(0);

        glBindVertexArray(zGrid);
        drawZGrid(flatShader);
        glBindVertexArray(0);

        mat4 model;


        
        glBindVertexArray(lightCubeVAO);
        //when 1 is pressed
        if (currentScenario == 1) {
            //display rods
            for (unsigned int i = 0; i < allRods.size(); i++)
            {
                for (int a = 1; a < 10 - 1; a++) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, allRods[i].getCurrentPoint(a));
                    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                    flatShader.setMat4("model", model);
                    glDrawArrays(drawMode, 0, 36);

                    //check 

                    //check if point is hit if length(point,centerSpher)<radius
                    //if point is hit displace by radius/length(point,centerSphere)*unit vector(point,centerSphere)


                }
                allRods[i].update();
            }
            //display nots
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, nots[i][j].getPosition());
                    model = glm::scale(model, glm::vec3(1.f)); // Make it a smaller cube
                    flatShader.setMat4("model", model);
                    glDrawArrays(drawMode, 0, 36);

                    nots[i][j].update();
                }
            }
            //display sphere
            glBindVertexArray(0);
            glBindVertexArray(sphereVAO);
            
                x0 = sphere.getPosition().x;
                y0 = sphere.getPosition().y;
               
                model = glm::mat4(1.0f);
                model = glm::translate(model, sphere.getPosition());
                model = glm::scale(model, glm::vec3(10.f)); // Make it a smaller cube
                flatShader.setMat4("model", model);
                glDrawElements(drawMode, 2880, GL_UNSIGNED_INT,nullptr);
                
                sphere.EulerUpdate();
                //check if sphere keeps moving in negative z axis and check collision
                if (sphere.getVelocity().z > 0) {
                    sphere.setForce(vec3(0, 0, nots[2][2].GetTotalForce().z));
                    if (glm::distance(nots[2][2].getPosition(), sphere.getPosition()) < sphere.getRadius()) {
                        nots[2][2].setPosition(curr + vec3(0, 0, sphere.getPosition().z + sphere.getRadius()));

                    }
                   

                    //collision for rods
                    for (unsigned int i = 0; i < allRods.size(); i++)
                    {
                        for (int a = 1; a < 10 - 1; a++) {
                            if (glm::distance(allRods[i].getCurrentPoint(a), sphere.getPosition()) < sphere.getRadius()) {


                                float z = sqrt(powf(sphere.getRadius(), 2) - powf(allRods[i].getCurrentPoint(a).x - (nots[2][2].getPosition().x), 2) - powf(allRods[i].getCurrentPoint(a).y - ((nots[2][2].getPosition().x)), 2));
                                /*vec3 unitVector = glm::normalize(allRods[i].getCurrentPoint(a) - sphere.getPosition());
                                float length = glm::distance(allRods[i].getCurrentPoint(a), sphere.getPosition());*/
                                // allRods[i].setCurrentPoint(a, allRods[i].getCurrentPoint(a)+((sphere.getRadius() / length) * unitVector)+vec3(0,0,5));

                                allRods[i].setCurrentPoint(a, vec3(allRods[i].getCurrentPoint(a).x, allRods[i].getCurrentPoint(a).y, allRods[i].GetPresetPoint(a).z + z + sphere.getPosition().z));

                                //cout << z;

                            }
                        }
                    }
                }
                //move it back
                else {
                    sphere.setForce(vec3(0, 0, nots[2][2].GetTotalForce().z-0.6));
                    //debug purposes
                    //cout << nots[2][2].GetTotalForce().z;
                    
                }
                //debug print msgs
               /* cout << nots[1][1].getStretchForce().x << "," << nots[1][1].getStretchForce().y << "," << nots[1][1].getStretchForce().z << "\n";
                cout << nots[1][1].GetTotalForce().x << "," << nots[1][1].GetTotalForce().y << "," << nots[1][1].GetTotalForce().z << "\n";
                cout << nots[1][1].KHorizontal().x << "," << nots[1][1].KHorizontal().y << "," << nots[1][1].KHorizontal().z << "\n";
                cout << nots[1][1].KVertical().x << "," << nots[1][1].KVertical().y << "," << nots[1][1].KVertical().z << "\n";*/

                //cout << (sphere.getVelocity().z>0);
                //rod.displayEnergy();




            
        }
        //simple rod scenario
        else if (currentScenario == 2) {
            //debug messages
            /*for (int a = 1; a < nbOfPoints - 1; a++) {

                vec3 ya = rod.forceAtPoint(a);
                vec3 ye = rod.stretchForce(a);


                cout << "Total" << a << "\n";
                cout << ya.x << "," << ya.y << "," << ya.z << "\n";
                cout << "Stretch" << a << "\n";
                cout << ye.x << "," << ye.y << "," << ye.z << "\n";

            }*/
            //display points of rod
            for (int a = 1; a < 10 - 1; a++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, vec3(0, 0, -5));
                model = glm::translate(model, rod.getCurrentPoint(a));
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                flatShader.setMat4("model", model);
                glDrawArrays(drawMode, 0, 36);
            }

            rod.update();
            model = glm::mat4(1.0f);
            model = glm::translate(model, vec3(0, 0, -5));
            model = glm::translate(model, rod.getCurrentPoint(0));
            model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
            flatShader.setMat4("model", model);
            glDrawArrays(drawMode, 0, 36);
            //display fixed nots
            model = glm::mat4(1.0f);
            model = glm::translate(model, vec3(0, 0, -5));
            model = glm::translate(model, rod.getCurrentPoint(10-1));
            model = glm::scale(model, glm::vec3(0.3f)); // Make it a smaller cube
            flatShader.setMat4("model", model);
            glDrawArrays(drawMode, 0, 36);
        }
        //scenario for basketball
        else if (currentScenario == 3) {

        //display the rods of the basket
        for (unsigned int i = 0; i < allRodsBasket.size()-9; i++)
        {
            
            for (int a = 1; a < allRodsBasket[i].nbOfPoints-1; a++) {
                if (dontDraw.find(i) == dontDraw.end()) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, allRodsBasket[i].getCurrentPoint(a));
                    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                    flatShader.setMat4("model", model);
                    glDrawArrays(drawMode, 0, 36);
                }
                //check 

                //check if point is hit if length(point,centerSpher)<radius
                //if point is hit displace by radius/length(point,centerSphere)*unit vector(point,centerSphere)
               /* if (allRodsBasket[i].forceAtPoint(a) != vec3(0)) {
                    cout << allRodsBasket[i].forceAtPoint(a).x << "," << allRodsBasket[i].forceAtPoint(a).y << "," << allRodsBasket[i].forceAtPoint(a).z << "\n";
                    cout << i << "," << a << "\n";
                }*/
            }
            allRodsBasket[i].update();

        }
        //display nots of basket
            for (int i = 0; i < nbLvls-1; i++) {

                for (int j = 0; j < nbNotLvl; j++) {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, basketNots[i][j].getPosition());
                    
                    model = glm::scale(model, glm::vec3(0.5f));
                    flatShader.setMat4("model", model);
                    glDrawArrays(drawMode, 0, 36);

                    basketNots[i][j].update();
                }
            }
            
           
            //display sphere
            glBindVertexArray(0);
            glBindVertexArray(sphereVAO);

            
            model = glm::mat4(1.0f);
            model = glm::translate(model, sphere2.getPosition());
            model = glm::scale(model, glm::vec3(sphere2.getRadius())); 
            flatShader.setMat4("model", model);
            glDrawElements(drawMode, 2880, GL_UNSIGNED_INT, nullptr);
            //sphere2.setPosition(vec3(sphere2.getPosition().x, yPos, sphere2.getPosition().z));

            sphere2.EulerUpdate();



            //collision detection for rods
            for (unsigned int i = 0; i < allRodsBasket.size(); i++)
            {
                for (int a = 1; a < allRodsBasket[i].nbOfPoints - 1; a++) {
                    if (glm::distance(allRodsBasket[i].getCurrentPoint(a), sphere2.getPosition()) < sphere2.getRadius()) {

                        vec3 unitVector = glm::normalize(allRodsBasket[i].getCurrentPoint(a) - sphere2.getPosition());

                        unitVector *= sphere2.getRadius();

                        
                        
                        allRodsBasket[i].setCurrentPoint(a, vec3(unitVector.x+sphere2.getPosition().x,allRodsBasket[i].getCurrentPoint(a).y,unitVector.z+sphere2.getPosition().z));

                    }
                }
            }
            //collision detection for nots
            for (int i = 1; i < nbLvls-1; i++) {
                for (int j = 0; j < nbNotLvl; j++) {
                    if (glm::distance(basketNots[i][j].getPosition(), sphere2.getPosition()) < sphere2.getRadius()) {

                        vec3 unitVector = glm::normalize(basketNots[i][j].getPosition() - sphere2.getPosition());

                        unitVector *= sphere2.getRadius();



                         basketNots[i][j].setPosition(vec3(unitVector.x + sphere2.getPosition().x, basketNots[i][j].getPosition().y, unitVector.z + sphere2.getPosition().z));

                    }
                }
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    // deallocate buffers
  

    glfwTerminate();
    return 0;
}





// every GLFW callback is down here
// ------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        inputState[RIGHT_CLICK] = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        inputState[LEFT_CLICK] = true;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
       inputState[MIDDLE_CLICK] = true;
        


    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        inputState[RIGHT_CLICK] = false;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        inputState[LEFT_CLICK] = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        inputState[MIDDLE_CLICK] = false;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (inputState[LEFT_CLICK])
    {
        float xoffset = xpos - lastX;
        lastX = xpos;
        lastY = ypos;
        camera.mouseLook(xoffset, 0);
    }

    else if (inputState[MIDDLE_CLICK])
    {
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.mouseLook(0, yoffset);
    }

    else if (inputState[RIGHT_CLICK])
    {
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.mouseZoom(yoffset);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        currentScenario = 1;
        camera.moveAndLookAt(cameraPositions[0], objectPositions[0]);
    }

    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        currentScenario = 2;
        camera.moveAndLookAt(cameraPositions[1], objectPositions[1]);
    }

    else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        currentScenario = 3;
        camera.moveAndLookAt(cameraPositions[2], objectPositions[2]);
    }

    // Translate camera
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        inputState[W] = true;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        inputState[S] = true;
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        inputState[A] = true;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        inputState[D] = true;
    else if (key == SHIFT && action == GLFW_PRESS)
        inputState[SHIFT] = true;

    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        inputState[W] = false;
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        inputState[S] = false;
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        inputState[A] = false;
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        inputState[D] = false;
    else if (key == SHIFT && action == GLFW_RELEASE)
        inputState[SHIFT] = false;

    //toggle cull Face
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        if (cullFace) {
            glDisable(GL_CULL_FACE);
            cullFace = !cullFace;
        }
        else {
            glEnable(GL_CULL_FACE);
            cullFace = !cullFace;
        }
    }

    //toggle shadow
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        shadow = !shadow;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        yPos--;
    }
    //toggle texture
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        texture = !texture;
    }


    // Reset camera
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
        camera.moveAndLookAt(vec3(0, 0, 0), vec3(0, 0, -1));


    // Change draw mode for objects
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        drawMode = GL_POINTS;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        drawMode = GL_LINES;
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        drawMode = GL_TRIANGLES;


}

void draw(GLsizei size)
{
    if (drawMode == GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, 0, size);
    else if (drawMode == GL_LINES)
        glDrawArrays(GL_LINES, 0, size);
    else if (drawMode == GL_POINTS)
        glDrawArrays(GL_POINTS, 0, size);
}

void handleKeyHold(float deltaTime) {
    if (inputState[W])
        camera.keysMove(Controls::FORWARD, deltaTime);
    if (inputState[S])
        camera.keysMove(Controls::BACKWARD, deltaTime);
    if (inputState[A])
        camera.keysMove(Controls::LEFT, deltaTime);
    if (inputState[D])
        camera.keysMove(Controls::RIGHT, deltaTime);
 

}