DEBUG=y
############################
# Enable optional features #
############################
USE_HAMLIB=y
USE_FLDIGI=n
USE_SOUNDMODEM=n

all: world
bins := bin/hamchat

clean_files = hamchat.log
distclean_files = log/hamchat.pid etc/hamchat.db etc/users/*.db etc/channels/*.db

subdirs += ext/ardop1

##################
# Compiler Flags #
##################
CXX = g++
CFLAGS := -I./include -std=gnu++20
LDFLAGS := -lsqlite3 -lev

# Optimization/debugging/warnings
CFLAGS += -Wno-unused-function -Wno-unused-variable -Wno-format-truncation

ifeq (${DEBUG},y)
CFLAGS += -g -Wall -ansi -pedantic
else
CFLAGS += -O3
endif

# buiild time autoconfiguration, based on user selections above...
ifeq (${USE_HAMLIB},y)
LDFLAGS += -lhamlib 
endif
ifeq (${USE_FLDIGI},y)
NEED_XMLRPC=y
endif
ifeq (${NEED_XMLRPC},y)
LDFLAGS += -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_util
endif

##################################################################

hamchat_objs += bot.o
#hamchat_objs += cmdline.o
hamchat_objs += config.o
hamchat_objs += daemon.o
hamchat_objs += database.o
hamchat_objs += dict.o
hamchat_objs += file.o
hamchat_objs += heartbeat.o
hamchat_objs += logger.o
hamchat_objs += main.o
hamchat_objs += maths.o
#hamchat_objs += module.o
hamchat_objs += rig.o
#hamchat_objs += rig_flrig.o
hamchat_objs += rig_hamlib.o
# Provides rig controls/status in channels
hamchat_objs += rig_irc_channel.o
#hamchat_objs += rig_serial_ptt.o
hamchat_objs += routing.o
hamchat_objs += statistics.o
hamchat_objs += strings.o

#########
# Audio #
#########
#hamchat_objs += audio.o
#hamchat_objs += audio_alsa.o
#hamchat_objs += audio_pulse.o

#################
# Spot Reporting#
#################
hamchat_objs += spotting.o
hamchat_objs += spotting_db.o
#hamchat_objs += spotting_pskreporter.o
#hamchat_objs += spotting_rbn.o

#######
# IRC #
#######
hamchat_objs += irc.o
hamchat_objs += irc_channel.o
hamchat_objs += irc_client.o		# for handling irc clients, NOT an IRC client...
hamchat_objs += irc_parser.o
hamchat_objs += m_admin.o
hamchat_objs += m_away.o
hamchat_objs += m_cap.o
hamchat_objs += m_die.o
hamchat_objs += m_help.o
hamchat_objs += m_info.o
hamchat_objs += m_invite.o
hamchat_objs += m_join.o
hamchat_objs += m_kick.o
hamchat_objs += m_kill.o
hamchat_objs += m_list.o
hamchat_objs += m_lusers.o
hamchat_objs += m_mode.o
hamchat_objs += m_motd.o
hamchat_objs += m_names.o
hamchat_objs += m_nick.o
hamchat_objs += m_notice.o
hamchat_objs += m_oper.o
hamchat_objs += m_part.o
hamchat_objs += m_pass.o
hamchat_objs += m_ping.o
hamchat_objs += m_pong.o
hamchat_objs += m_privmsg.o
hamchat_objs += m_quit.o
hamchat_objs += m_rehash.o
hamchat_objs += m_stats.o
hamchat_objs += m_time.o
hamchat_objs += m_topic.o
hamchat_objs += m_unknown.o
hamchat_objs += m_user.o
hamchat_objs += m_userhost.o
hamchat_objs += m_version.o
hamchat_objs += m_who.o
hamchat_objs += m_whois.o
hamchat_objs += m_whowas.o

#####################
# Transports/Modems #
#####################
hamchat_objs += modem.o
hamchat_objs += modem_ardop.o
#hamchat_objs += modem_bell103.o
#hamchat_objs += modem_bell202.o
#hamchat_objs += modem_js8call.o
#hamchat_objs += modem_psk31.o
hamchat_objs += transport.o
hamchat_objs += transport_socket.o
#hamchat_objs += transport_kiss.o
hamchat_objs += transport_modem.o
hamchat_objs += wireproto_simple.o
ifeq (${USE_FLDIGI},y)
hamchat_objs += modem_fldigi.o
endif

ifeq (${USE_SOUNDMODEM},y)
hamchat_objs += modem_soundmodem.o
endif

#############################################################
# allows us to force rebuild below, if any headers change...
headers := $(wildcard include/*.h)

# expand to full paths
hamchat_real_objs = $(foreach x,${hamchat_objs},obj/${x})

# add files to cleanup...
clean_files += ${bins} ${hamchat_objs} ${hamchat_real_objs}

world: subdirs-all ${bins}

ARDOP_BIN := ext/ardop1/ardopc

bin/hamchat: ${ARDOP_BIN} ${hamchat_real_objs} GNUmakefile ${headers} etc/hamchat.db
	@echo "[link] $@"
	@${CXX} -o $@ ${hamchat_real_objs} ${LDFLAGS}

clean: subdirs-clean
	@echo "Cleaning source tree..."
	@${RM} ${clean_files} *.log
	@echo "Source tree ready for build"

distclean: clean subdirs-distclean
	@echo "Cleaning for release..."
	@${RM} ${distclean_files}
	@echo "Release clean completed!"

# attempt to install missing packages
build-dep:
	@echo "* Trying to install dependancies via apt"
	@sudo ./install-deps

obj/%.o: src/%.cpp ${headers}
	@echo "[build] $<"
	@${CXX} ${CFLAGS} -o $@ -c $<

etc/hamchat.db: etc/hamchat.sql
	sqlite3 etc/hamchat.db < etc/hamchat.sql

start:
	./scripts/launch-ardop
	./bin/hamchat

gdb: bin/hamchat stop
#	./scripts/launch-ardop
	gdb ./bin/hamchat -ex run

stop:
	@if [  -f log/hamchat.pid ]; then \
	    echo "*** killing hamchat!"; \
	    kill -TERM $(shell cat log/hamchat.pid) || true; \
	fi
	kill -TERM $(shell cat log/ardop.pid) || true
	${RM} log/hamchat.pid log/ardop.pid

ext/ardop1/ardopc:
	${MAKE} -C ext/ardop1

##################
# subdirectories #
##################
subdirs-all:
	@for i in ${subdirs}; do \
		echo "building $$i"; \
		${MAKE} -C $$i; \
	done

subdirs-clean:
	@for i in ${subdirs}; do \
		echo "cleaning $$i"; \
		${MAKE} -C $$i clean; \
	done

subdirs-distclean:
	@for i in ${subdirs}; do \
		echo "distcleaning $$i"; \
		${MAKE} -C $$i distclean || true; \
	done
