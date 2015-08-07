require 'date'
require 'erb'
require 'yaml'
include ERB::Util
$no_errors = true
$sketch_extensions = ['.gif', '.png', '.mp3']
$default_description_text = 'Write your description here'

#api
task :copy do
	load_config
	if validate
		copy_templates
		copy_sketches
		copy_media
		generate_files
	else
		puts 'Please fix these errors before copying'
	end
end

task :validate do
	load_config
	validate
end

task :status do
	load_config
	print_all_status
end

task :st do
	load_config
	print_all_status
end

task :deploy, :datestring do |t, args|
	load_config
	if args[:datestring] == nil
		puts 'This command deploys one sketch at a time, identified by it\'s date'
		puts 'Usage: rake deploy[today]'
		puts 'or:    rake deploy[yesterday]'
		puts 'or:    rake deploy[yyyy-mm-dd]'
	elsif args[:datestring] == 'today'
		deploy_all Date::today.strftime
	elsif args[:datestring] == 'yesterday'
		deploy_all Date::today.prev_day.strftime
	else
		datestring = args[:datestring].strip.chomp('\n')
		if datestring == ''
			puts 'Usage: rake deploy[yyyy-mm-dd]'
		else
			deploy_all datestring
		end
	end
end

#config
def load_config
	config = YAML.load_file('config.yml')
	$site_name = config['site_name']
	$current_asset_dir = config['current_asset_dir']
	$current_sketch_repo = config['current_sketch_repo']
	$sketches_dir = config['sketches_dir']
	$templates_dir = config['templates_dir']
	$jekyll_dir = config['jekyll_dir']
	$live_url = config['live_url']
	$github_org_url = config['github_org_url']
end

#tasks
def print_all_status
	puts "Sketches status:\n================\n"
	execute_silent "git status"
	puts "\nAssets status:\n==============\n"
	execute_silent "cd assets/#$current_asset_dir && git status && cd ../.."
	puts "\nJekyll status:\n==============\n"
	execute_silent "cd #$jekyll_dir && git status && cd .."
end

def deploy_all datestring
	puts "\nDeploying sketch:\n================="
	execute "pwd && git add sketches/#{datestring} && git commit -m 'Adds sketch #{datestring}' && git push"
	puts "\nDeploying assets:\n================="
	execute "cd assets/#{$current_asset_dir} && pwd && git add */#{datestring}.* && git commit -m 'Adds sketch #{datestring}' && git push"
	puts "\nDeploying jekyll:\n================="
	execute "cd #$jekyll_dir && pwd && git add app/_posts/#{datestring}-sketch.md && git commit -m 'Adds sketch #{datestring}' && git push && grunt deploy"
end

def validate
	validate_unexpected_assets_not_present && validate_expected_asset_present && validate_snippet_and_description
end

def validate_expected_asset_present
	valid = true
	sketch_dirs.each do |sketch_dir|
		expected_asset_selector = "#{$sketches_dir}#{sketch_dir}/bin/data/out/#{sketch_dir}.*"
		if Dir.glob(expected_asset_selector).empty?
			puts "WARNING: No valid asset found in 'bin/data/out' of sketch #{sketch_dir}"
			valid = false
		end
	end
	valid
end

def validate_unexpected_assets_not_present
	valid = true
	sketch_dirs.each do |sketch_dir|
		all_asset_selector = "#{$sketches_dir}#{sketch_dir}/bin/data/out/*.*"
		Dir.glob(all_asset_selector).select do |entry|
			if File.basename(entry, '.*') != sketch_dir || !$sketch_extensions.include?(File.extname(entry))
				puts "WARNING: Unexpected asset '#{File.basename entry}' found in 'bin/data/out' of sketch #{sketch_dir}"
				valid = false
			end
		end
	end
	valid
end

def validate_snippet_and_description
	valid = true
	sketch_dirs.each do |sketch_dir|
		dest_cpp_path = "sketches/#{sketch_dir}/src/ofApp.cpp"
		unless File.exist?(dest_cpp_path)
			contents = read_snippet_contents sketch_dir
			if contents.nil? || contents.empty? || contents == $default_description_text
				puts "WARNING: Snippet not found for sketch #{sketch_dir}"
				valid = false
			end
			contents = read_description_contents sketch_dir
			if contents.nil? || contents.empty? || contents == $default_description_text
				puts "WARNING: Description not found for sketch #{sketch_dir}"
				valid = false
			end
		end
	end
	valid
end

def sketch_dirs
	Dir.entries($sketches_dir).select do |entry|
		File.directory? File.join($sketches_dir, entry) and !(entry == '.' || entry == '..')
	end
end

def copy_templates
	starttime = Time.now
	print "Copying openFrameworks templates... "
	execute_silent "rsync -ru #{$templates_dir} templates"
	endtime = Time.now
	print "completed in #{endtime - starttime} seconds.\n"
end

def copy_sketches
	starttime = Time.now
	print "Copying openFrameworks sketches... "
	execute_silent "rsync -ru #{$sketches_dir} sketches"
	endtime = Time.now
	print "Completed in #{endtime - starttime} seconds.\n"
end

def copy_media
	starttime = Time.now
	print "Copying generated openFrameworks media... "
	execute_silent "mv -f sketches/*/bin/data/out/* assets/#$current_asset_dir/openFrameworks/"
	endtime = Time.now
	print "completed in #{endtime - starttime} seconds.\n"
end

def generate_files
	starttime = Time.now
	print "Generating jekyll post files... "
	Dir.foreach "assets/#$current_asset_dir/openFrameworks/" do |filename|
		if filename.end_with? '.gif'
			filename.slice! '.gif'
			generate_post filename, 'gif'
			generate_readme filename, 'gif'
		end
		if filename.end_with? '.mp3'
			filename.slice! '.mp3'
			generate_post filename, 'mp3'
			generate_readme filename, 'mp3'
		end
	end
	endtime = Time.now
	print "completed in #{endtime - starttime} seconds.\n"
end

def generate_post datestring, ext
	filepath = "#$jekyll_dir/app/_posts/#{datestring}-sketch.md"
	unless File.exist?(filepath)
		file = open(filepath, 'w')
		file.write(post_file_contents datestring, ext)
		file.close
	end
end

def generate_readme datestring, ext
	filepath = "sketches/#{datestring}/README.md"
	unless File.exist?(filepath)
		file = open(filepath, 'w')
		file.write(readme_file_contents datestring, ext)
		file.close
	end
end

def execute commandstring
	puts "Executing command: #{commandstring}"
	execute_silent commandstring
end

def execute_silent commandstring
	if $no_errors
		$no_errors = system commandstring
		unless $no_errors
			puts "\nEXECUTION ERROR. Subsequent commands will be ignored\n"
		end
	else
		puts "\nEXECUTION SKIPPED due to previous error\n"
	end
end

#string builders
def post_file_contents datestring, ext
	<<-eos
---
layout: post
title:  "Sketch #{datestring}"
date:   #{datestring}
---
<div class="code">
    <ul>
		<li><a href="{% post_url #{datestring}-sketch %}">permalink</a></li>
		<li><a href="#$github_org_url/#$current_sketch_repo/tree/master/sketches/#{datestring}">code</a></li>
		<li><a href="#" class="snippet-button">show snippet</a></li>
	</ul>
    <pre class="snippet">
        <code class="cpp">#{read_snippet_contents(datestring)}</code>
    </pre>
</div>
<p class="description">#{read_description_contents(datestring)}</p>
#{ext == 'gif' ? render_post_gif(datestring) : render_post_mp3(datestring)}
eos
end

def readme_file_contents datestring, ext
	<<-eos
Sketch #{datestring}
--
This subfolder of the [#$current_sketch_repo repo](#$github_org_url/#$current_sketch_repo) is the root of an individual openFrameworks sketch. It contains the full source code used to generate this sketch:

#{ext == 'gif' ? render_readme_gif(datestring) : render_readme_mp3(datestring)}

This source code is published automatically along with each sketch I add to [#$site_name](#$live_url). Here is a [permalink to this sketch](#$live_url/sketch-#{reverse datestring}/) on the #$site_name site.

Run this yourself
--
If you are running [openFrameworks via XCode](http://openframeworks.cc/download/) on a Mac you can just clone this directory and launch the `xcodeproj`. If you do that you should see something similar to the sketch above.

Addons
--
If the sketch uses any [addons](http://www.ofxaddons.com/unsorted) you don't already have you will have to clone them. Note that this readme was auto-generated and so doesn't list the addon dependencies. However you can figure out which addons you need pretty easily.

In XCode you will see a panel like this. Expand the folders under `addons` until you can see some of the source files underneath.

![How to find missing addon dependencies](../../images/dependencies.png)

In the example above, the addon `ofxLayerMask` is missing (it's source files are red), but `ofxGifEncoder` is present.

Versions
--
Note that openFrameworks doesn't have a great system for versioning addons. If you are getting results that look different to the gif above, or it won't compile, you may have cloned newer versions of some addons than were originally used to generate the sketch.

In that case you should clone the addon(s) at the most recent commit before the sketch date. Not ideal, but you will only have to do it rarely.

Yes, openFrameworks could use a good equivalent of [bundler](http://bundler.io/). You should write one!
eos
end

def read_snippet_contents sketch_dir
	regex = /\/\* Snippet begin \*\/(.*?)\/\* Snippet end \*\//m
	read_cpp_contents sketch_dir, regex
end

def read_description_contents sketch_dir
	regex = /\/\* Begin description\n\{(.*?)\}\nEnd description \*\//m
	read_cpp_contents sketch_dir, regex
end

def read_cpp_contents sketch_dir, regex
	source_cpp_path = "#{$sketches_dir}#{sketch_dir}/src/ofApp.cpp"
	if File.exist?(source_cpp_path)
		file = open(source_cpp_path, 'r')
		contents = file.read
		file.close
		selected = contents[regex, 1]
		if selected.nil?
			selected
		else
			html_escape(selected.strip.chomp('\n'))
		end
	else
		nil
	end
end

#string builder helpers
def reverse datestring
	date = DateTime.parse(datestring)
	date.strftime('%d-%m-%Y')
end

def raw_url datestring, ext
	"#$github_org_url/#$current_asset_dir/blob/master/openFrameworks/#{datestring}.#{ext}?raw=true"
end

def render_post_gif datestring
	"![Daily sketch](#{raw_url datestring, 'gif'})"
end

def render_post_mp3 datestring
	<<-eos
<p>
	<img src="#{raw_url datestring, 'png'}" alt="Sketch #{datestring}">
	<audio controls>
		<source src="#{raw_url datestring, 'mp3'}" type="audio/mpeg">
		Your browser does not support the audio element.
	</audio>
</p>
eos
end

def render_readme_gif datestring
	"![Sketch #{datestring}](#{raw_url datestring, 'gif'})"
end

def render_readme_mp3 datestring
	<<-eos
![Sketch #{datestring}](#{raw_url datestring, 'png'})
[Listen to the sketch on #$site_name](#$live_url/sketch-#{reverse datestring}/)"
eos
end