cmake_minimum_required(VERSION 3.7.2)

project(hello-camkes-1 C)

DeclareTutorialsCAmkESComponent(Manager TEMPLATE_SOURCES components/Manager/src/manager.c)
DeclareTutorialsCAmkESComponent(Lead TEMPLATE_SOURCES components/Lead/src/lead.c)
DeclareTutorialsCAmkESComponent(Member TEMPLATE_SOURCES components/Member/src/member.c)

DeclareTutorialsCAmkESRootserver(hello-camkes-1.camkes ADL_IS_TEMPLATE
    TEMPLATE_SOURCES interfaces/TalktoLead.camkes interfaces/TalktoMember.camkes
    TUTORIAL_SOURCES components/Manager/Manager.camkes components/Lead/Lead.camkes components/Member/Member.camkes
)