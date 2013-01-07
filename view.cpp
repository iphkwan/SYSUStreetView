#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <png.h>
#include <cstdio>
#include <errno.h>
#include <malloc.h>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <cstdlib>
using namespace std;

struct NextNode {
    NextNode(string filename, int deg)
        :img_file(filename), deg(deg) {}
    string img_file;
    int deg;
};

struct ImageNode {
    ImageNode(string filename, int dev)
        :img_file(filename), deviate(dev) {}
    string img_file;
    int deviate; // 偏转角（图片中的北方与Z轴的夹角）
    /** 用于修正图片偏转，使用时用nowdeg += deviate
     *            |
     *            |
     *            |
     *            |
     *            |
     *  ----------.----------->
     *           /|         X
     *          / |
     *         /deviate
     *        /   |
     *           Zv nowdeg=0
     */
    vector<NextNode> next;
};

const double PI = acos(-1);

GLfloat len = 5.0f;
GLfloat radius = 2.0f;

GLuint texname;
GLubyte *textureImage;
int imgW, imgH;
bool imgHasAlpha;
int nowdeg = 0; // 0度为正北
int nowdeviate = 0;

vector<ImageNode> imgAdjList;
map<string, int> imgIndex;
int currentImg = 0;

void loadImage(const char *filename);

bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;

    if ((fp = fopen(name, "rb")) == NULL)
        return false;

    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we knowdeg if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
            NULL, NULL, NULL);

    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }

    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        return false;
    }

    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
        return false;
    }

    /* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);

    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);

    outWidth = info_ptr->width;
    outHeight = info_ptr->height;
    switch (info_ptr->color_type) {
        case PNG_COLOR_TYPE_RGBA:
            outHasAlpha = true;
            break;
        case PNG_COLOR_TYPE_RGB:
            outHasAlpha = false;
            break;
        default:
            std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            return false;
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*) malloc(row_bytes * outHeight);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (int i = 0; i < outHeight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (outHeight-1-i)), row_pointers[i], row_bytes);
    }

    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    /* Close the file */
    fclose(fp);

    /* That's it */
    return true;
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);
    for (GLfloat i = 0.0f; i <= 2 * PI; i += 0.01f) {
        double pos1 = i / (2 * PI);
        double pos2 = (i + 0.01) / (2 * PI);
        glTexCoord2f(/*pos1 > 1 ? 1 : */pos1, 0.0); glVertex3f(cos(i) * radius, -len / 2, sin(i) * radius);
        glTexCoord2f(/*pos2 > 1 ? 1 : */pos2, 0.0); glVertex3f(cos(i + 0.01) * radius, -len / 2, sin(i + 0.01) * radius);
        glTexCoord2f(/*pos2 > 1 ? 1 : */pos2, 1.0); glVertex3f(cos(i + 0.01) * radius, len / 2, sin(i + 0.01) * radius);
        glTexCoord2f(/*pos1 > 1 ? 1 : */pos1, 1.0); glVertex3f(cos(i) * radius, len / 2, sin(i) * radius);
    }
    glEnd();

    glFlush();
    glutSwapBuffers();
}

static double lookatx = 1.0, lookatz = 0.0;
static int scrw, scrh;
void reshape(int w, int h) {
    scrw = w;
    scrh = h;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 0, lookatx, 0.0, lookatz, 0.0, 1.0, 0.0);
}

double degToRad(int d) {
    return (d / 180.0) * PI;
}
/*
void passiveMotionFunc(int x, int y) {
    if ( x > (scrw / 2) ) 
        nowdeg = (nowdeg + 2) % 360;
    else {
        nowdeg = (nowdeg - 2) % 360;
        if (nowdeg < 0) nowdeg = 360;
    }

    //cout << "nowdeg=" << nowdeg << endl;

    lookatx = cos(degToRad(nowdeg));
    lookatz = sin(degToRad(nowdeg));

    //cout << "x=" << x << " scrw=" << scrw << endl;
    //cout << lookatx << "  " << lookatz << endl;
    glLoadIdentity();

    gluLookAt(0, 0, 0, lookatx, 0.0, lookatz, 0.0, 1.0, 0.0);
    glutPostRedisplay();
}
*/

void changeImage(int deg) {
    vector<NextNode>& next = imgAdjList[currentImg].next;
    vector<NextNode>::iterator itr = next.begin();
    for (; itr != next.end(); ++ itr) {
        if (abs(itr->deg - deg) <= 10) {
            break;
        }
    }

    if (itr != next.end()) {
        free(textureImage);

	    loadImage(itr->img_file.c_str());
        nowdeviate = imgAdjList[currentImg].deviate;

        currentImg = imgIndex[itr->img_file];

        cout << "nowdeviate=" << nowdeviate << endl;
    }
}

void keyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        nowdeg = (nowdeg - 2) % 360;
        if (nowdeg < 0) nowdeg = 360;
        break;
    case GLUT_KEY_RIGHT:
        nowdeg = (nowdeg + 2) % 360;
        break;
    case GLUT_KEY_UP:
        changeImage(nowdeg);
        break;
    default:
        break;
    }
    cout << "Current deg = " << nowdeg << endl;

    lookatx = cos(degToRad((nowdeg + nowdeviate) % 360));
    lookatz = sin(degToRad((nowdeg + nowdeviate) % 360));
    glLoadIdentity();

    gluLookAt(0, 0, 0, lookatx, 0.0, lookatz, 0.0, 1.0, 0.0);
    glutPostRedisplay();
}

void loadImage(const char *filename) {
    bool valid = loadPngImage(filename, imgW, imgH, imgHasAlpha, &textureImage);
    if (!valid) {
        perror(filename);
        exit(1);
    }

    double fac = len / imgH;
    radius = imgW / 2.0 / PI * fac;


    cout << "imgW=" << imgW << " imgH=" << imgH << " radius=" << radius << endl;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(2, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexImage2D(GL_TEXTURE_2D, 0, imgHasAlpha ? 4 : 3, imgW,
                imgH, 0, imgHasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
                textureImage);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void readConfigFile(const char *filename) {
    fstream conf;
    conf.open(filename, ios::in);
    char buffer[256];
    while (!conf.eof()) {
        conf.getline(buffer, 256);
        // Get Filename
        char *imgnod = strtok(buffer, " :;");
        if (imgnod == NULL) continue;
        string filename(imgnod);
        imgnod = strtok(NULL, " :;");
        int deviate = atoi(imgnod);
        imgIndex[filename] = imgAdjList.size();
        ImageNode node(filename, deviate);

        cout << "nodename=" << filename << " deviate=" << deviate << " Adj: ";

        imgnod = strtok(NULL, " :;");
        while (imgnod != NULL) {
            string name(imgnod);
            imgnod = strtok(NULL, " :;");
            int deg = atoi(imgnod);

            cout << "name=" << name << " deg=" << deg << ";";

            node.next.push_back(NextNode(name, deg));

            imgnod = strtok(NULL, " :;");
        }
        cout << endl;

        imgAdjList.push_back(node);
    }

    cout << "Read " << imgAdjList.size() << " Images" << endl;
    conf.close();
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);

    // Initial Image
    if (imgAdjList.size()) {
        loadImage(imgAdjList[0].img_file.c_str());
        nowdeviate = imgAdjList[0].deviate;

        lookatx = cos(degToRad((nowdeg + nowdeviate) % 360));
        lookatz = sin(degToRad((nowdeg + nowdeviate) % 360));
    }

    glLoadIdentity();
}

int main(int argc, char **argv) {
    cout << "PI=" << PI << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Panorama Viewer");
    if (argc != 2) {
        cout << "You must input a configure file" << endl;
        exit(1);
    }
    readConfigFile(argv[1]);
    //loadImage(argv[1]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    glutMainLoop();
}
