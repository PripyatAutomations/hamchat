---
--- A rough database schema for storing state
drop table if exists irc_channels;
drop table if exists irc_users;
drop table if exists irc_chan_members;
drop table if exists irc_msgqueue;
drop table if exists transport_routes;

create table irc_channels (
   chan_id INTEGER PRIMARY KEY AUTOINCREMENT,
   chan_name VARCHAR,
   chan_topic VARCHAR,
   chan_created INTEGER,
-- channel modes: op/voice/moderated/invite-only
   chan_mode INTEGER,
   chan_owner INTEGER
);

create table irc_users (
   user_id INTEGER PRIMARY KEY AUTOINCREMENT,
-- user's callsign (primary identifier)
   user_callsign VARCHAR,
-- last used nickname by the callsign
   user_nick VARCHAR,
-- hashed password, if local user
   user_pass VARCHAR,
-- when was user last online?
   user_lastseen INTEGER
);

-- Used for NAMES, etc
create table irc_chan_members (
   member_id INTEGER PRIMARY KEY AUTOINCREMENT,
   member_chan_id INTEGER,
   member_user_id INTEGER,
--- time_t the user joined channel
   member_joined INTEGER,
--- channel flags (op/voice/muted)
   member_flags INTEGER
);

---------------
--- queued outgoing messages
create table irc_msgqueue (
   queue_msgid INTEGER PRIMARY KEY AUTOINCREMENT,
   -- refers to irc_users
   queue_sender INTEGER,
   queue_dest INTEGER,
   -- time sent
   queue_sent INTEGER,
   -- last time transmitted
   queue_last_tx INTEGER,
   -- retries remaining
   queue_retries INTEGER
);
