all: world

CXX = g++
CFLAGS := -g -Wall -ansi -pedantic -I./include -std=gnu++20
CFLAGS += -Wno-unused-function -Wno-unused-variable -Wno-format-truncation
LDFLAGS := -lsqlite3 -lev -lhamlib

bins := hamchat
#subdirs += ext/ft8_lib
clean_files = hamchat.log
distclean_files = hamchat.pid etc/hamchat.db etc/users/*.db etc/channels/*.db
#hamchat_objs += audio.o
#hamchat_objs += audio_alsa.o
#hamchat_objs += audio_pulse.o
#hamchat_objs += cmdline.o
hamchat_objs += config.o
hamchat_objs += daemon.o
hamchat_objs += database.o
hamchat_objs += dict.o
hamchat_objs += file.o
hamchat_objs += heartbeat.o
hamchat_objs += irc.o
hamchat_objs += irc_channel.o
hamchat_objs += irc_client.o		# for handling irc clients, NOT an IRC client...
hamchat_objs += irc_parser.o
#hamchat_objs += list.o
hamchat_objs += logger.o
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
hamchat_objs += main.o
hamchat_objs += modem.o
hamchat_objs += modem_ardop.o
#hamchat_objs += modem_bell103.o
#hamchat_objs += modem_bell202.o
#hamchat_objs += modem_js8call.o
#hamchat_objs += modem_psk31.o
#hamchat_objs += module.o
hamchat_objs += rig.o
#hamchat_objs += rig_flrig.o
hamchat_objs += rig_hamlib.o
# Provides rig controls/status in channels
hamchat_objs += rig_irc_channel.o
#hamchat_objs += rig_serial_ptt.o
hamchat_objs += routing.o
hamchat_objs += spotting.o
hamchat_objs += spotting_db.o
#hamchat_objs += spotting_pskreporter.o
#hamchat_objs += spotting_rbn.o
hamchat_objs += statistics.o
hamchat_objs += transport.o
hamchat_objs += transport_socket.o
#hamchat_objs += transport_kiss.o
hamchat_objs += transport_modem.o

hamchat_real_objs = $(foreach x,${hamchat_objs},obj/${x})

# add files to cleanup...
clean_files += ${bins} ${hamchat_objs} ${hamchat_real_objs}
headers := $(wildcard include/*.h)

world: subdirs-all ${bins}

#ARDOP_BIN := ext/ARDOPOFDM/ardopofdm
hamchat: ${ARDOP_BIN} ${hamchat_real_objs} GNUmakefile ${headers} etc/hamchat.db
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
	./launch-ardop
	./hamchat

gdb: hamchat stop
	gdb ./hamchat -ex run

stop:
	@if [  -f hamchat.pid ]; then \
	    echo "*** killing hamchat!"; \
	    kill -TERM $(shell cat hamchat.pid) || true; \
	fi
	kill -TERM $(shell cat ardop.pid) || true
	${RM} hamchat.pid ardop.pid

ext/ARDOPOFDM/ardopofdm:
	${MAKE} -C ext/ARDOPOFDM

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
