//
// Created by jian.yeung on 2020/10/27.
//

#ifndef BBQVIDEO_TRIANGLEFILTER_H
#define BBQVIDEO_TRIANGLEFILTER_H

#include <GLES3/gl3.h>
#include "BaseFilter.h"

class TriangleFilter : public BaseFilter {
private:
    const char* TRIANGLE_FILTER_TAG = "TriangleFilter";
    //parameter
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    //Handle
    GLuint mPositionHandle;
    GLuint mColorHandle;

public:
    TriangleFilter();
    ~TriangleFilter();

    void setUp() override;
    void tearDown() override;
    void initVAO();
    void onSurfaceCreated(ANativeWindow *nativeWindow) override;
    void onSurfaceChanged(ANativeWindow *nativeWindow, int format, int width, int height) override;
    void updateMVPMatrix() override;
    void draw() override;
    void onDestroy() override;
};


#endif //BBQVIDEO_TRIANGLEFILTER_H
