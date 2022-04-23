#ifndef __TEST_OPENGL__
#define __TEST_OPENGL__

#include <vector>
#include <functional>
#include <string>

namespace test {

// 所有测试类的基类，只提供了用于测试的接口
class Test {
    public:
        Test() {}
        virtual ~Test() {}

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}
};



class TestMenu : public Test {
    public:
        TestMenu(Test*& currentTestPointer);
        ~TestMenu() {}

        void OnImGuiRender() override;

        // C++ 的模板不支持分离编译，由此会在链接的阶段出现一些函数无法找到的错误
        // 目前的解决方法只有两种，第一种是将实现也写在 .h 文件里面。第二种就是做显式实例化
        // 因此具体的实现只能写在头文件里面。而不能使用分离写法
        template <typename T>
        void RegisterTest(const std::string& name) {
#ifdef DEBUG
            printf("[%s:L%d] Register test %s\n", __FUNCTION__, __LINE__, name.c_str());
#endif /* DEBUG */

            // 这里只是进行注册，只有在 mTests 的相关的元素被访问的时候，才会真正的去执行创建 test 实例的过程
            m_Tests.push_back(std::make_pair(name, [](){ return new T(); } ) );
        }

    private: 
        Test*& m_CurrentTest;
        // 这里存储所有的测试实例的名称和其对应的构建函数。从这里我们可以构建所有的测试实例
        std::vector<std::pair<std::string, std::function<Test*()> > > m_Tests;
};

}   /* namespace test */

#endif  /* __TEST_OPENGL__ */