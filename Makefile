# Makefile generated by MakefileGenerator.cs
# *DO NOT EDIT*

UNREALROOTPATH = /opt/unreal-engine
GAMEPROJECTFILE =/home/smol_cat/git/escape_room_game/escape_room.uproject

TARGETS = \
	escape_room-Linux-DebugGame  \
	escape_room-Linux-Shipping  \
	escape_room \
	escape_roomEditor-Linux-DebugGame  \
	escape_roomEditor-Linux-Shipping  \
	escape_roomEditor \
	UnrealEditor-Linux-DebugGame  \
	UnrealEditor-Linux-Shipping  \
	UnrealEditor \
	UnrealGame-Linux-DebugGame  \
	UnrealGame-Linux-Shipping  \
	UnrealGame\
	configure

BUILD = bash "$(UNREALROOTPATH)/Engine/Build/BatchFiles/Linux/Build.sh"
PROJECTBUILD = bash "$(UNREALROOTPATH)/Engine/Build/BatchFiles/Linux/RunMono.sh" "$(UNREALROOTPATH)/Engine/Binaries/DotNET/UnrealBuildTool.exe"

all: StandardSet

RequiredTools: CrashReportClient-Linux-Shipping CrashReportClientEditor-Linux-Shipping ShaderCompileWorker UnrealLightmass EpicWebHelper-Linux-Shipping

StandardSet: RequiredTools UnrealFrontend UnrealEditor UnrealInsights

DebugSet: RequiredTools UnrealFrontend-Linux-Debug UnrealEditor-Linux-Debug


escape_room-Linux-DebugGame:
	 $(PROJECTBUILD) escape_room Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

escape_room-Linux-Shipping:
	 $(PROJECTBUILD) escape_room Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

escape_room:
	 $(PROJECTBUILD) escape_room Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

escape_roomEditor-Linux-DebugGame:
	 $(PROJECTBUILD) escape_roomEditor Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

escape_roomEditor-Linux-Shipping:
	 $(PROJECTBUILD) escape_roomEditor Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

escape_roomEditor:
	 $(PROJECTBUILD) escape_roomEditor Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealEditor-Linux-DebugGame:
	 $(BUILD) UnrealEditor Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealEditor-Linux-Shipping:
	 $(BUILD) UnrealEditor Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealEditor:
	 $(BUILD) UnrealEditor Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealGame-Linux-DebugGame:
	 $(BUILD) UnrealGame Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealGame-Linux-Shipping:
	 $(BUILD) UnrealGame Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

UnrealGame:
	 $(BUILD) UnrealGame Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

configure:
	xbuild /property:Configuration=Development /verbosity:quiet /nologo "$(UNREALROOTPATH)/Engine/Source/Programs/UnrealBuildTool/UnrealBuildTool.csproj"
	$(PROJECTBUILD) -projectfiles -project="\"$(GAMEPROJECTFILE)\"" -game -engine 

.PHONY: $(TARGETS)
