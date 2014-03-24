#!/usr/bin/ruby

require "sinatra"

server_root = "/var/www/localhost"
server_doc = "#{server_root}/htdocs"
server_cgi = "#{server_root}/cgi-bin"

set :public_folder, server_doc

get "/*/" do |path|
	if File.exists?("#{server_doc}/#{path}/index.html")
		File.open("#{server_doc}/#{path}/index.html", "r").read
	else
		status 404
		"<h1>Error: 404<br />File not found!</h1>"
	end
end

post "/cgi-bin/*" do |script|
	ret = IO.popen("#{server_cgi}/#{script}", "r+") do |io|
		io.print request.env["rack.input"].read
		io.close_write
		io.read
	end
	ret = ret.split("\r\n\r\n", 2)
	ret[0].each_line do |line|
		tmp = line.split(": ", 2)
		headers tmp[0] => tmp[1]
	end
	ret[1]
end
