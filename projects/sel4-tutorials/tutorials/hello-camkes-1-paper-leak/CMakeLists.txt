cmake_minimum_required(VERSION 3.7.2)

project(hello-camkes-1 C)

DeclareTutorialsCAmkESComponent(Professor TEMPLATE_SOURCES components/Professor/src/professor.c)
DeclareTutorialsCAmkESComponent(TA TEMPLATE_SOURCES components/TA/src/ta.c)
DeclareTutorialsCAmkESComponent(Student TEMPLATE_SOURCES components/Student/src/student.c)

DeclareTutorialsCAmkESRootserver(hello-camkes-1.camkes ADL_IS_TEMPLATE
    TEMPLATE_SOURCES interfaces/SendPaper.camkes interfaces/TalktoStudent.camkes
    TUTORIAL_SOURCES components/Professor/Professor.camkes components/TA/TA.camkes components/Student/Student.camkes
)