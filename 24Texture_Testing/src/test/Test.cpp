#include "Test.h"
#include "imgui.h"

namespace test {

class TestClearColor;

TestMenu::TestMenu(Test*& currentTestPointer) 
    :m_CurrentTest(currentTestPointer) {
}


void TestMenu::OnImGuiRender() {
    for (auto& test : m_Tests) {
        if (ImGui::Button(test.first.c_str()))
            m_CurrentTest = test.second();
    }
}

}   /* namespace test */