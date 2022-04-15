#ifndef __TESTCLEARCOLOR_OPENGL__
#define __TESTCLEARCOLOR_OPENGL__

#include "Test.h"


namespace test {

class TestClearColor : public Test {
    public:
        TestClearColor();
        ~TestClearColor();

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    
    private:
        float m_ClearColor[4];
};


}   /* namespace test */

#endif /* __TESTCLEARCOLOR_OPENGL__ */