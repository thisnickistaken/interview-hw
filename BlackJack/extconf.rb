#!/usr/bin/ruby

require "mkmf"

extension_name = "blackjack"
dir_config(extension_name)

have_header("blackjack.h") or abort "blackjack.h is missing!"
have_library("blackjack") or abort "libblackjack is missing!"

create_makefile(extension_name)
