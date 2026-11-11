// ECM2209270 Assignment Question2
// A 3D soccer ball is designed.
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

// --- Constants & Structures ---
struct Point3 {
    float x, y, z;
};

struct Face {
    std::vector<Point3> vertices;
    bool isPentagon;
    Point3 normal;
    Point3 center;
};

std::vector<Face> ballFaces;
float animTime = 0.0f;
float ballHeight = 0.0f;
float spinAngle = 0.0f;

// Light settings
GLfloat lightPos[] = { 10.0f, 20.0f, 10.0f, 1.0f };
GLfloat floorShadow[4][4];

// --- Math Helpers ---
Point3 normalize(Point3 p) {
    float len = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (len == 0) return {0,0,0};
    return { p.x / len, p.y / len, p.z / len };
}

Point3 add(Point3 a, Point3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
Point3 scale(Point3 p, float s) { return { p.x * s, p.y * s, p.z * s }; }
float dot(Point3 a, Point3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

void computeFaceData(Face& f) {
    // 1. Center
    f.center = {0, 0, 0};
    for(auto& v : f.vertices) f.center = add(f.center, v);
    f.center = scale(f.center, 1.0f / f.vertices.size());

    // 2. Normal
    Point3 v1 = { f.vertices[1].x - f.vertices[0].x, f.vertices[1].y - f.vertices[0].y, f.vertices[1].z - f.vertices[0].z };
    Point3 v2 = { f.vertices[2].x - f.vertices[0].x, f.vertices[2].y - f.vertices[0].y, f.vertices[2].z - f.vertices[0].z };

    f.normal.x = v1.y * v2.z - v1.z * v2.y;
    f.normal.y = v1.z * v2.x - v1.x * v2.z;
    f.normal.z = v1.x * v2.y - v1.y * v2.x;

    // 3. Fix Normal Direction (Ensure it points OUT)
    if (dot(f.normal, f.center) < 0) {
        f.normal = scale(f.normal, -1.0f);
    }
    f.normal = normalize(f.normal);
}

// --- Geometry Generation ---
void generateSoccerBall() {
    float X = 0.525731112119133606f;
    float Z = 0.850650808352039932f;

    Point3 vdata[12] = {
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };

    int tindices[20][3] = {
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
    };

    // Hexagons
    for (int i = 0; i < 20; ++i) {
        Point3 p0 = vdata[tindices[i][0]];
        Point3 p1 = vdata[tindices[i][1]];
        Point3 p2 = vdata[tindices[i][2]];

        Face hex;
        hex.isPentagon = false;

        hex.vertices.push_back(normalize(add(p0, scale(add(p1, scale(p0, -1.0f)), 1.0f/3.0f))));
        hex.vertices.push_back(normalize(add(p0, scale(add(p1, scale(p0, -1.0f)), 2.0f/3.0f))));
        hex.vertices.push_back(normalize(add(p1, scale(add(p2, scale(p1, -1.0f)), 1.0f/3.0f))));
        hex.vertices.push_back(normalize(add(p1, scale(add(p2, scale(p1, -1.0f)), 2.0f/3.0f))));
        hex.vertices.push_back(normalize(add(p2, scale(add(p0, scale(p2, -1.0f)), 1.0f/3.0f))));
        hex.vertices.push_back(normalize(add(p2, scale(add(p0, scale(p2, -1.0f)), 2.0f/3.0f))));

        computeFaceData(hex);
        ballFaces.push_back(hex);
    }

    // Pentagons
    for (int i = 0; i < 12; ++i) {
        Point3 center = vdata[i];
        Face pent;
        pent.isPentagon = true;

        struct VertAngle { Point3 v; float angle; };
        std::vector<VertAngle> candidates;

        Point3 axis = normalize(center);
        Point3 ref = {0, 1, 0};
        if (fabs(dot(axis, ref)) > 0.9f) ref = {1, 0, 0};
        Point3 u = normalize(add(ref, scale(axis, -dot(ref, axis))));
        Point3 v_axis = {
             axis.y * u.z - axis.z * u.y,
             axis.z * u.x - axis.x * u.z,
             axis.x * u.y - axis.y * u.x
        };

        for(const auto& face : ballFaces) {
            for(const auto& v : face.vertices) {
                float dist = sqrt(pow(v.x - center.x, 2) + pow(v.y - center.y, 2) + pow(v.z - center.z, 2));
                if (dist < 0.7f) {
                    bool unique = true;
                    for(auto& c : candidates) {
                         if(sqrt(pow(c.v.x - v.x, 2) + pow(c.v.y - v.y, 2) + pow(c.v.z - v.z, 2)) < 0.001f) unique = false;
                    }
                    if(unique) {
                        Point3 vec = {v.x - center.x, v.y - center.y, v.z - center.z};
                        float ang = atan2(dot(vec, v_axis), dot(vec, u));
                        candidates.push_back({v, ang});
                    }
                }
            }
        }

        // Sort Vertices
        for(size_t a=0; a<candidates.size(); a++) {
            for(size_t b=a+1; b<candidates.size(); b++) {
                if(candidates[a].angle > candidates[b].angle) std::swap(candidates[a], candidates[b]);
            }
        }

        for(auto& c : candidates) pent.vertices.push_back(c.v);
        computeFaceData(pent);
        ballFaces.push_back(pent);
    }
}

// --- Shadow ---
void createShadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4]) {
    GLfloat dot = groundplane[0] * lightpos[0] + groundplane[1] * lightpos[1] + groundplane[2] * lightpos[2] + groundplane[3] * lightpos[3];
    shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
    shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
    shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
    shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];
    shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
    shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
    shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
    shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];
    shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
    shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
    shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
    shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];
    shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
    shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
    shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
    shadowMat[3][3] = dot - lightpos[3] * groundplane[3];
}

void drawBall() {
    for (const auto& face : ballFaces) {
        if (face.isPentagon) {
            // Solid Black
            GLfloat mat_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            GLfloat mat_diff[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            GLfloat mat_spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
        } else {
            // White
            GLfloat mat_amb[] = { 0.6f, 0.6f, 0.6f, 1.0f };
            GLfloat mat_diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            GLfloat mat_spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
        }

        glBegin(GL_POLYGON);
        glNormal3f(face.normal.x, face.normal.y, face.normal.z);
        for (const auto& v : face.vertices) {
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
}

void drawFloor() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Grid
    glBegin(GL_LINES);
    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    for(float i = -10; i <= 10; i+=1.0f) {
        glVertex3f(i, -2.0f, -10.0f);
        glVertex3f(i, -2.0f, 10.0f);
        glVertex3f(-10.0f, -2.0f, i);
        glVertex3f(10.0f, -2.0f, i);
    }
    glEnd();

    // Grass
    glColor4f(0.13f, 0.55f, 0.13f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -2.01f, -10.0f);
    glVertex3f(-10.0f, -2.01f, 10.0f);
    glVertex3f(10.0f, -2.01f, 10.0f);
    glVertex3f(10.0f, -2.01f, -10.0f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 3.0, 7.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawFloor();

    // Shadow
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);

    glPushMatrix();
        glMultMatrixf((GLfloat *)floorShadow);
        glTranslatef(0.0f, ballHeight, 0.0f);
        glRotatef(spinAngle, 1.0f, 0.0f, -1.0f);
        for(const auto& face : ballFaces) {
             glBegin(GL_POLYGON);
             for(const auto& v : face.vertices) glVertex3f(v.x, v.y, v.z);
             glEnd();
        }
    glPopMatrix();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Ball
    glPushMatrix();
        glTranslatef(0.0f, ballHeight, 0.0f);
        glRotatef(spinAngle, 1.0f, 0.0f, -1.0f);
        drawBall();
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value) {
    animTime += 0.04f;
    ballHeight = fabs(sin(animTime)) * 2.5f;
    spinAngle += 1.5f;
    if (spinAngle > 360.0f) spinAngle -= 360.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);

    glShadeModel(GL_FLAT);

    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

    generateSoccerBall();

    GLfloat groundPlane[4] = { 0.0f, 1.0f, 0.0f, 2.0f };
    createShadowMatrix(floorShadow, groundPlane, lightPos);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Soccer Ball");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
