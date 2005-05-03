#
# Half-Life ProSDK 2.0 hl_i386.so Makefile for i386 Linux
#
# April 2000 by Leon Hartwig (jehannum@planethalflife.com)
#

DLLNAME=ns

ARCH=i386

#make sure this is the correct compiler for your system
CC=gcc
LD=gcc

LIBBASE=../../../libraries/current

DLL_SRCDIR=../dlls
GAME_SHARED_SRCDIR = ../game_shared
MOD_SRCDIR = ../mod
PARTICLES_SRCDIR = ../particles
PM_SHARED_SRCDIR=../pm_shared
TEXT_SRCDIR = ../textrep
UTIL_SRCDIR = ../util

DLL_OBJDIR=$(DLL_SRCDIR)/obj
GAME_SHARED_OBJDIR=$(GAME_SHARED_SRCDIR)/obj
MOD_OBJDIR=$(MOD_SRCDIR)/obj
PARTICLES_OBJDIR=$(PARTICLES_SRCDIR)/obj
PM_SHARED_OBJDIR=$(PM_SHARED_SRCDIR)/obj
TEXT_OBJDIR=$(TEXT_SRCDIR)/obj
UTIL_OBJDIR=$(UTIL_SRCDIR)/obj
OUTPUT_DIR=../../hlds_l/ns/dlls

BASE_CFLAGS=-Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -DAVH_SERVER -DLINUX  -DVALVE_DLL -DQUIVER -DVOXEL -DQUAKE2 -DDEDICATED -DSWDS -D_STLP_USE_GLIBC

#full optimization
CFLAGS=$(BASE_CFLAGS) -w -Wall -nostdinc++ -ffor-scope -fPIC -mcpu=i486 -O3 -pipe  -funroll-loops -fdelayed-branch -malign-loops=4 -malign-jumps=4 -malign-functions=4  

#use these when debugging 
#CFLAGS=$(BASE_CFLAGS) -fPIC -fno-for-scope   

# add base directory (CGC)

INCLUDEDIRS=-I$(LIBBASE) -I. -I$(LIBBASE)/include -I$(LIBBASE)/include/stlport -I$(LIBBASE)/include/particle -I$(LIBBASE)/include/lua -I../dlls -I../engine -I../common -I../mod -I../game_shared -I../pm_shared -I.. -I/usr/include/c++/3.3/i486-linux -I/usr/include/c++/3.3 -I/usr/include/c++ 
LINKDIRS=  -L$(LIBBASE)/release

SHLIBEXT=so

LDPRELIBS= $(CFLAGS) -shared \
           -Wl,-Map,ns_map.txt \
           $(LINKDIRS)

LDPOSTLIBS= -Wl,-Bstatic -lstlport_gcc -lstdc++ -lsupc++ -llua -llualib\
	-Wl,-Bdynamic -lm -lgcc -lgcc_eh
# -lgcc -lgcc_eh -lelf
DO_CC=$(CC) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

$(CURL_OBJDIR)/%.o: $(CURL_SRCDIR)/%.c
	$(DO_CC)

$(DLL_OBJDIR)/%.o: $(DLL_SRCDIR)/%.cpp
	$(DO_CC)

$(GAME_SHARED_OBJDIR)/%.o: $(GAME_SHARED_SRCDIR)/%.cpp
	$(DO_CC)

$(MOD_OBJDIR)/%.o: $(MOD_SRCDIR)/%.cpp
	$(DO_CC)

$(PARTICLES_OBJDIR)/%.o: $(PARTICLES_SRCDIR)/%.cpp
	$(DO_CC)

$(PM_SHARED_OBJDIR)/%.o: $(PM_SHARED_SRCDIR)/%.cpp
	$(DO_CC)

$(TEXT_OBJDIR)/%.o: $(TEXT_SRCDIR)/%.cpp
	$(DO_CC)

$(UTIL_OBJDIR)/%.o: $(UTIL_SRCDIR)/%.cpp
	$(DO_CC)

OBJ = \
	$(DLL_OBJDIR)/animating.o \
	$(DLL_OBJDIR)/animation.o \
	$(DLL_OBJDIR)/bmodels.o \
	$(DLL_OBJDIR)/buttons.o \
	$(DLL_OBJDIR)/cbase.o \
	$(DLL_OBJDIR)/client.o \
	$(DLL_OBJDIR)/combat.o \
	$(DLL_OBJDIR)/doors.o \
	$(DLL_OBJDIR)/effects.o \
	$(DLL_OBJDIR)/egon.o \
	$(DLL_OBJDIR)/explode.o \
	$(DLL_OBJDIR)/func_break.o \
	$(DLL_OBJDIR)/func_tank.o \
	$(DLL_OBJDIR)/game.o \
	$(DLL_OBJDIR)/gamerules.o \
	$(DLL_OBJDIR)/gauss.o \
	$(DLL_OBJDIR)/globals.o \
	$(DLL_OBJDIR)/ggrenade.o \
	$(DLL_OBJDIR)/h_ai.o \
	$(DLL_OBJDIR)/h_battery.o \
	$(DLL_OBJDIR)/h_cycler.o \
	$(DLL_OBJDIR)/h_export.o \
	$(DLL_OBJDIR)/items.o \
	$(DLL_OBJDIR)/lights.o \
	$(DLL_OBJDIR)/maprules.o \
	$(DLL_OBJDIR)/mpstubb.o \
	$(DLL_OBJDIR)/multiplay_gamerules.o \
	$(DLL_OBJDIR)/observer.o \
	$(DLL_OBJDIR)/pathcorner.o \
	$(DLL_OBJDIR)/plane.o \
	$(DLL_OBJDIR)/plats.o \
	$(DLL_OBJDIR)/player.o \
	$(DLL_OBJDIR)/satchel.o \
	$(DLL_OBJDIR)/shotgun.o \
	$(DLL_OBJDIR)/singleplay_gamerules.o \
	$(DLL_OBJDIR)/skill.o \
	$(DLL_OBJDIR)/sound.o \
	$(DLL_OBJDIR)/soundent.o \
	$(DLL_OBJDIR)/spectator.o \
	$(DLL_OBJDIR)/squeakgrenade.o \
	$(DLL_OBJDIR)/subs.o \
	$(DLL_OBJDIR)/teamplay_gamerules.o \
	$(DLL_OBJDIR)/triggers.o \
	$(DLL_OBJDIR)/turret.o \
	$(DLL_OBJDIR)/util.o \
	$(DLL_OBJDIR)/weapons.o \
	$(DLL_OBJDIR)/world.o \
	$(DLL_OBJDIR)/xen.o \
	$(GAME_SHARED_OBJDIR)/voice_banmgr.o \
	$(GAME_SHARED_OBJDIR)/voice_gamemgr.o \
	$(PM_SHARED_OBJDIR)/pm_debug.o \
	$(PM_SHARED_OBJDIR)/pm_math.o \
	$(PM_SHARED_OBJDIR)/pm_shared.o \
	$(MOD_OBJDIR)/AnimationUtil.o \
	$(MOD_OBJDIR)/AvHAcidRocketGun.o \
	$(MOD_OBJDIR)/AvHAlienAbilities.o \
	$(MOD_OBJDIR)/AvHAlienEquipment.o \
	$(MOD_OBJDIR)/AvHAlienTurret.o \
	$(MOD_OBJDIR)/AvHAlienWeapon.o \
	$(MOD_OBJDIR)/AvHAssert.o \
	$(MOD_OBJDIR)/AvHBaseBuildable.o \
	$(MOD_OBJDIR)/AvHBaseInfoLocation.o \
	$(MOD_OBJDIR)/AvHBasePlayerWeapon.o \
	$(MOD_OBJDIR)/AvHBileBombGun.o \
	$(MOD_OBJDIR)/AvHBite.o \
	$(MOD_OBJDIR)/AvHBite2.o \
	$(MOD_OBJDIR)/AvHBlink.o \
	$(MOD_OBJDIR)/AvHBuildable.o \
	$(MOD_OBJDIR)/AvHBuildingGun.o \
	$(MOD_OBJDIR)/AvHClaws.o \
	$(MOD_OBJDIR)/AvHCloakable.o \
	$(MOD_OBJDIR)/AvHConsoleCommands.o \
	$(MOD_OBJDIR)/AvHConstants.o \
	$(MOD_OBJDIR)/AvHCombat.o \
	$(MOD_OBJDIR)/AvHDevour.o \
	$(MOD_OBJDIR)/AvHDivineWind.o \
	$(MOD_OBJDIR)/AvHEntities.o \
	$(MOD_OBJDIR)/AvHEntityHierarchy.o \
	$(MOD_OBJDIR)/AvHGamerules.o \
	$(MOD_OBJDIR)/AvHGrenade.o \
	$(MOD_OBJDIR)/AvHGrenadeGun.o \
	$(MOD_OBJDIR)/AvHHealingSpray.o \
	$(MOD_OBJDIR)/AvHHeavyMachineGun.o \
	$(MOD_OBJDIR)/AvHHive.o \
	$(MOD_OBJDIR)/AvHItemInfo.o \
	$(MOD_OBJDIR)/AvHKnife.o \
	$(MOD_OBJDIR)/AvHMachineGun.o \
	$(MOD_OBJDIR)/AvHMapExtents.o \
	$(MOD_OBJDIR)/AvHMarineEquipment.o \
	$(MOD_OBJDIR)/AvHMarineTurret.o \
	$(MOD_OBJDIR)/AvHMarineWeapon.o \
	$(MOD_OBJDIR)/AvHMetabolize.o \
	$(MOD_OBJDIR)/AvHMine.o \
	$(MOD_OBJDIR)/AvHMiniMap.o \
	$(MOD_OBJDIR)/AvHMovementUtil.o \
	$(MOD_OBJDIR)/AvHNetworkMessages.o \
	$(MOD_OBJDIR)/AvHOrder.o \
	$(MOD_OBJDIR)/AvHParasiteGun.o \
	$(MOD_OBJDIR)/AvHParticleSystem.o \
	$(MOD_OBJDIR)/AvHParticleSystemEntity.o \
	$(MOD_OBJDIR)/AvHParticleSystemManager.o \
	$(MOD_OBJDIR)/AvHParticleTemplate.o \
	$(MOD_OBJDIR)/AvHParticleTemplateServer.o \
	$(MOD_OBJDIR)/AvHPistol.o \
	$(MOD_OBJDIR)/AvHPlayer.o  \
	$(MOD_OBJDIR)/AvHPlayerUpgrade.o  \
	$(MOD_OBJDIR)/AvHPrimalScream.o \
	$(MOD_OBJDIR)/AvHPushableBuildable.o \
	$(MOD_OBJDIR)/AvHReinforceable.o \
	$(MOD_OBJDIR)/AvHResearchManager.o  \
	$(MOD_OBJDIR)/AvHScriptManager.o \
	$(MOD_OBJDIR)/AvHScriptServer.o \
	$(MOD_OBJDIR)/AvHScriptShared.o \
	$(MOD_OBJDIR)/AvHSelectionHelper.o \
	$(MOD_OBJDIR)/AvHServerPlayerData.o \
	$(MOD_OBJDIR)/AvHServerUtil.o \
	$(MOD_OBJDIR)/AvHSharedMovementInfo.o \
	$(MOD_OBJDIR)/AvHSharedUtil.o \
	$(MOD_OBJDIR)/AvHSiegeTurret.o \
	$(MOD_OBJDIR)/AvHSonicGun.o \
	$(MOD_OBJDIR)/AvHSoundListManager.o \
	$(MOD_OBJDIR)/AvHSpecials.o \
	$(MOD_OBJDIR)/AvHSpikeGun.o \
	$(MOD_OBJDIR)/AvHSpitGun.o \
	$(MOD_OBJDIR)/AvHSpores.o \
	$(MOD_OBJDIR)/AvHStomp.o \
	$(MOD_OBJDIR)/AvHSwipe.o \
	$(MOD_OBJDIR)/AvHTeam.o \
	$(MOD_OBJDIR)/AvHTechNode.o \
	$(MOD_OBJDIR)/AvHTurret.o \
	$(MOD_OBJDIR)/AvHTechTree.o \
	$(MOD_OBJDIR)/AvHUmbraGun.o \
	$(MOD_OBJDIR)/AvHVisibleBlipList.o \
	$(MOD_OBJDIR)/AvHVoiceHelper.o \
	$(MOD_OBJDIR)/AvHWebSpinner.o \
	$(MOD_OBJDIR)/AvHWeldable.o \
	$(MOD_OBJDIR)/AvHWelder.o \
	$(MOD_OBJDIR)/AvHWorldUpdate.o \
	$(MOD_OBJDIR)/AvHTechSlotManager.o \
	$(MOD_OBJDIR)/AvHSpawn.o \
	$(MOD_OBJDIR)/CollisionUtil.o \
	$(MOD_OBJDIR)/CollisionChecker.o \
	$(MOD_OBJDIR)/CollisionChecker_ServerOnly.o \
	$(MOD_OBJDIR)/NetworkMeter.o \
	$(TEXT_OBJDIR)/TRDescription.o \
	$(TEXT_OBJDIR)/TRFactory.o \
	$(UTIL_OBJDIR)/Checksum.o \
	$(UTIL_OBJDIR)/Balance.o \
	$(UTIL_OBJDIR)/LinuxSupport.o \
	$(UTIL_OBJDIR)/Mat3.o \
	$(UTIL_OBJDIR)/MathUtil.o \
	$(UTIL_OBJDIR)/Quat.o \
	$(UTIL_OBJDIR)/STLUtil.o \
	$(UTIL_OBJDIR)/Tokenizer.o

#	$(PARTICLES_OBJDIR)/action_api.o 
#	$(PARTICLES_OBJDIR)/actions.o 
#	$(PARTICLES_OBJDIR)/system.o 

$(DLLNAME)_$(ARCH).$(SHLIBEXT) : neat $(OBJ)
	$(LD) $(LDPRELIBS) -o $@ $(OBJ) $(LDPOSTLIBS) 
	release: $(DLLNAME)_$(ARCH).$(SHLIBEXT)
	strip $(DLLNAME)_$(ARCH).$(SHLIBEXT)
	zip $(DLLNAME)_$(ARCH).zip $(DLLNAME)_$(ARCH).$(SHLIBEXT)
curl:
	./build-curl.sh
neat:
	-mkdir $(CURL_OBJDIR)
	-mkdir $(DLL_OBJDIR)
	-mkdir $(GAME_SHARED_OBJDIR)
	-mkdir $(MOD_OBJDIR)
	-mkdir $(PARTICLES_OBJDIR)
	-mkdir $(PM_SHARED_OBJDIR)
	-mkdir $(TEXT_OBJDIR)
	-mkdir $(UTIL_OBJDIR)
	-mkdir $(OUTPUT_DIR)
clean:
	-rm -f $(OBJ)
	-rm -f $(DLLNAME)_$(ARCH).$(SHLIBEXT)
spotless: clean
	-rm -r $(CURL_OBJDIR)
	-rm -r $(DLL_OBJDIR)
	-rm -r $(GAME_SHARED_OBJDIR)
	-rm -r $(MOD_OBJDIR)
	-rm -r $(PARTICLES_OBJDIR)
	-rm -r $(PM_SHARED_OBJDIR)
	-rm -r $(TEXT_OBJDIR)
	-rm -r $(UTIL_OBJDIR)















































