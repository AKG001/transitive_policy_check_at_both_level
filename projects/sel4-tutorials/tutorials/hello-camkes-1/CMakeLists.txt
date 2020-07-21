cmake_minimum_required(VERSION 3.7.2)

project(hello-camkes-1 C)

DeclareTutorialsCAmkESComponent(Client TEMPLATE_SOURCES components/Client/src/client.c)
DeclareTutorialsCAmkESComponent(Server TEMPLATE_SOURCES components/Server/src/server.c)

DeclareCAmkESComponent(EmptyComponent)

DeclareTutorialsCAmkESRootserver(hello-camkes-1.camkes ADL_IS_TEMPLATE
    TEMPLATE_SOURCES interfaces/HelloSimple.camkes
    TUTORIAL_SOURCES components/Client/Client.camkes components/Server/Server.camkes
)
