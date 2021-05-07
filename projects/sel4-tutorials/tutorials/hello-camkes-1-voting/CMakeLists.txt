cmake_minimum_required(VERSION 3.7.2)

project(hello-camkes-1 C)

DeclareTutorialsCAmkESComponent(Voter1 TEMPLATE_SOURCES components/Voter1/src/voter1.c)
DeclareTutorialsCAmkESComponent(Voter2 TEMPLATE_SOURCES components/Voter2/src/voter2.c)
DeclareTutorialsCAmkESComponent(Voter3 TEMPLATE_SOURCES components/Voter3/src/voter3.c)
DeclareTutorialsCAmkESComponent(VotingMachine TEMPLATE_SOURCES components/VotingMachine/src/votingmachine.c)

DeclareTutorialsCAmkESRootserver(hello-camkes-1.camkes ADL_IS_TEMPLATE
    TEMPLATE_SOURCES interfaces/CastVote.camkes interfaces/TalktoVoter.camkes
    TUTORIAL_SOURCES components/Voter1/Voter1.camkes components/Voter2/Voter2.camkes components/Voter3/Voter3.camkes components/VotingMachine/VotingMachine.camkes 
)