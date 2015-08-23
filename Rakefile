require 'date'
require 'erb'
require 'yaml'
include ERB::Util
$no_errors = true
$sketch_extensions = ['.gif', '.png', '.mp3']
$template_options = {'g' => 'gifEncoder', 'a' => 'audioSequencer'}
$default_description_text = 'Write your description here'
$git_clean_dir = 'working directory clean'
$num_managed_repos = 2

#api
task :copy do
	load_config
	if validate
		check_for_new_month
		copy_sketches
		generate_files
	else
		puts 'Please fix these errors before copying'
	end
end

task :clone, [:source, :datestring] do |t, args|
	load_config

	source = args[:source] || ''
	source = source.size == 1 ? $template_options[source] : source

	dest = args[:datestring] || ''
	dest = dest == 'today' ? Date::today.strftime : dest
	dest = dest == 'yesterday' ? Date::today.prev_day.strftime : dest
	dest = dest.strip.chomp('\n')

	if dest == '' || !$template_options.has_value?(source)
		puts 'This command generates a new runnable blank sketch from a template'
		puts 'Usage: rake clone[source,destination]'
		puts 'or:    rake clone[gifEncoder,yesterday]'
		puts 'or:    rake clone[audioSequencer,yyyy-mm-dd]'
	else
		clone dest, source
	end
end

task :validate do
	load_config
	validate
end

task :status do
	load_config
	print_uncopied_sketches
	print_all_status
end

task :st do
	Rake::Task['status'].invoke
end

task :move do
	load_config
	increment_month
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
	$sketch_manager_repo = config['sketch_manager_repo']

	$current_month = config['current_month']
	$next_month = get_next_month
	$current_month_name = month_name $current_month
	$next_month_name = month_name $next_month

	$current_sketch_repo = "sketches-#$current_month"
	$sketches_dir = config['sketches_dir']
	$templates_dir = config['templates_dir']
	$jekyll_repo = config['jekyll_repo']
	$site_url = "http://#$jekyll_repo"
	$github_org_name = config['github_org_name']
end

def get_next_month
	current_month_date = DateTime.parse "#$current_month-01"
	next_month_date = current_month_date >> 1
	next_month = next_month_date.strftime '%Y-%m'
end

def month_name month_str
	month_date = DateTime.parse "#{month_str}-01"
	month_date.strftime '%B'
end

def increment_month
	if new_month_sketch_detected? && ready_for_month_switch?
		puts "Moving to new repo:\n===================\n"
		execute "sed -i '' 's/current_month: #$current_month/current_month: #$next_month/g' config.yml && git add config.yml && git commit -m 'Increments to #$next_month' && git push"
	else
		puts 'You are not ready to move. Try running \'rake copy\' for more detail.'
	end
end

#tasks
def print_all_status
	puts "Sketch repo status:\n===================\n"
	puts "Remote: https://github.com/#$github_org_name/sketches-#$current_month\n"
	puts "Local:  sketches/sketches-#$current_month\n\n"
	execute_silent "cd sketches/#$current_sketch_repo && git status && cd ../.."
	puts "\nJekyll repo status:\n===================\n"
	puts "Remote: https://github.com/#$github_org_name/#$jekyll_repo\n"
	puts "Local:  #$jekyll_repo\n\n"
	execute_silent "cd #$jekyll_repo && git status && cd .."
end

def deploy_all datestring
	puts "\nDeploying sketch:\n================="
	execute "cd sketches/#$current_sketch_repo && pwd && git add #{datestring} && git commit -m 'Adds sketch #{datestring}' && git push & cd ../.."
	puts "\nDeploying jekyll:\n================="
	execute "cd #$jekyll_repo && pwd && git add app/_posts/#{datestring}-sketch.md && git commit -m 'Adds sketch #{datestring}' && git push && grunt deploy"
end

def clone datestring, source
	dirpath = "#$sketches_dir/#{datestring}"
	if Dir.exist?(dirpath)
		puts "WARNING: Sketch #{datestring} already exists... aborting."
	else
		#copy the template
		execute_silent "rsync -ru #$templates_dir/example-#{source} #$sketches_dir"

		#rename files
		xcodeproj = "#$sketches_dir/example-#{source}/example-#{source}.xcodeproj"
		execute_silent "mv #{xcodeproj}/project.xcworkspace/xcshareddata/example-#{source}.xccheckout #{xcodeproj}/project.xcworkspace/xcshareddata/#{datestring}.xccheckout"
		execute_silent "mv #{xcodeproj}/xcshareddata/xcschemes/example-#{source}\\\ Debug.xcscheme   #{xcodeproj}/xcshareddata/xcschemes/#{datestring}\\\ Debug.xcscheme"
		execute_silent "mv #{xcodeproj}/xcshareddata/xcschemes/example-#{source}\\\ Release.xcscheme #{xcodeproj}/xcshareddata/xcschemes/#{datestring}\\\ Release.xcscheme"
		execute_silent "mv #{xcodeproj} #$sketches_dir/example-#{source}/#{datestring}.xcodeproj"
		execute_silent "mv #$sketches_dir/example-#{source} #$sketches_dir/#{datestring}"

		#recursive rewrite of references to old filenames
		execute_silent "cd #$sketches_dir/#{datestring}/#{datestring}.xcodeproj && LC_ALL=C find . -path '*.*' -type f -exec sed -i '' -e 's/example-#{source}/#{datestring}/g' {} +"

		#clear user data dirs
		execute_silent "rm -rf #{xcodeproj}/project.xcworkspace/xcuserdata"
		execute_silent "rm -rf #{xcodeproj}/xcuserdata"
		execute_silent "rm -rf #$sketches_dir/#{datestring}/bin/data/AudioUnitPresets/.trash/"

		#clear generated binaries (note that .app files can be packages)
		execute_silent "rm -rf #$sketches_dir/#{datestring}/bin/*.app"
		$sketch_extensions.each do |ext|
			execute_silent "rm -f #$sketches_dir/#{datestring}/bin/data/out/*#{ext}"
		end

		cpp_path = "#$sketches_dir/#{datestring}/src/ofApp.cpp"
		contents = File.read(cpp_path)
		file = File.new(cpp_path, "w+")
		File.open(file, "a") do |file|
			file.write contents.gsub(/[\n\r]*.*\/\/.*/, '')
		end

		puts "Created sketch #{datestring} from example-#{source}."
	end
end

def validate
	validate_unexpected_assets_not_present && validate_expected_asset_present && validate_snippet_and_description
end

def validate_expected_asset_present
	valid = true
	sketch_dirs.each do |sketch_dir|
		expected_asset_selector = "#{$sketches_dir}/#{sketch_dir}/bin/data/out/#{sketch_dir}.*"
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
		all_asset_selector = "#{$sketches_dir}/#{sketch_dir}/bin/data/out/*.*"
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
		already_copied_file = Dir.glob "sketches/*/#{sketch_dir}/src/ofApp.cpp"
		if already_copied_file.empty?
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

def check_for_new_month
	if new_month_sketch_detected?
		if ready_for_month_switch?
			puts "You are ready to switch from your #$current_month_name repo to a new #$next_month_name repo!"
			puts "1. Go to https://github.com/organizations/#$github_org_name/repositories/new"
			puts "2. Enter sketches-#$next_month as the repository name and click 'create repository'"
			puts "3. Run 'rake move' to update the manager config"
			puts "The next time you run 'rake copy' everything will be working with the new month's repo"
		else
			puts "WARNING: One or more sketches for #$next_month_name were detected, but will not be copied yet. To copy them, please deploy all #$current_month_name sketches and run 'rake copy' again."
		end
	end
end

def new_month_sketch_detected?
	uncopied_sketches($next_month).length > 0
end

def ready_for_month_switch?
	uncopied_sketches($current_month).length == 0 && !undeployed_sketches_exist?
end

def print_uncopied_sketches
	puts "Source sketches dir:\n====================\n"
	current_month_sketches = uncopied_sketches $current_month
	if current_month_sketches.size == 0
		puts 'No sketches found waiting to be copied.'
	else
		puts 'The following openFrameworks sketches are ready to be copied:'
		puts current_month_sketches
		puts
		validate
	end
	puts
end

def uncopied_sketches target_month
	found_uncopied = Array.new
	Dir.glob "#$sketches_dir/#{target_month}*" do |dirpath|
		sketch_dir = File.basename dirpath
		target_filepath = "sketches/sketches-#{target_month}/#{sketch_dir}/src/ofApp.cpp"
		unless File.exist?(target_filepath)
			found_uncopied.push sketch_dir
		end
	end
	found_uncopied
end

def undeployed_sketches_exist?
	system_output = `rake status`
	return system_output.scan($git_clean_dir).length != $num_managed_repos
end

def copy_sketches
	starttime = Time.now
	print "Copying openFrameworks sketches... "
	execute_silent "rsync -ru #$sketches_dir/#$current_month* sketches/#$current_sketch_repo"
	endtime = Time.now
	print "completed in #{endtime - starttime} seconds.\n"
end

def generate_files
	starttime = Time.now
	print "Generating jekyll post files... "
	Dir.glob "sketches/#$current_sketch_repo/*/bin/data/out/*.*" do |filename|
		if filename.end_with? '.gif'
			filename = File.basename(filename, '.gif')
			generate_post filename, 'gif'
			generate_readme filename, 'gif'
		end
		if filename.end_with? '.mp3'
			filename = File.basename(filename, '.gif')
			generate_post filename, 'mp3'
			generate_readme filename, 'mp3'
		end
	end
	endtime = Time.now
	print "completed in #{endtime - starttime} seconds.\n"
end

def generate_post datestring, ext
	filepath = "#$jekyll_repo/app/_posts/#{datestring}-sketch.md"
	unless File.exist?(filepath)
		file = open(filepath, 'w')
		file.write(post_file_contents datestring, ext)
		file.close
	end
end

def generate_readme datestring, ext
	filepath = "sketches/#$current_sketch_repo/#{datestring}/README.md"
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
		<li><a href="https://github.com/#$github_org_name/#$current_sketch_repo/tree/master/#{datestring}">code</a></li>
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
This subfolder of the [#$current_sketch_repo repo](https://github.com/#$github_org_name/#$current_sketch_repo) is the root of an individual openFrameworks sketch. It contains the full source code used to generate this sketch:

#{ext == 'gif' ? render_readme_gif(datestring) : render_readme_mp3(datestring)}

This source code is published automatically along with each sketch I add to [#$site_name](#$site_url). Here is a [permalink to this sketch](#$site_url/sketch-#{reverse datestring}/) on the #$site_name site.

Run this yourself
--
If you are running [openFrameworks via XCode](http://openframeworks.cc/download/) on a Mac you can just clone this directory and launch the `xcodeproj`. If you do that you should see something similar to the sketch above.

Addons
--
If the sketch uses any [addons](http://www.ofxaddons.com/unsorted) you don't already have you will have to clone them. Note that this readme was auto-generated and so doesn't list the addon dependencies. However you can figure out which addons you need pretty easily.

In XCode you will see a panel like this. Expand the folders under `addons` until you can see some of the source files underneath.

![How to find missing addon dependencies](https://github.com/#$github_org_name/#$sketch_manager_repo/blob/master/images/dependencies.png?raw=true)

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
	source_cpp_path = "#{$sketches_dir}/#{sketch_dir}/src/ofApp.cpp"
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
	"https://github.com/#$github_org_name/#$current_sketch_repo/blob/master/#{datestring}/bin/data/out/#{datestring}.#{ext}?raw=true"
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
[Listen to the sketch on #$site_name](#$site_url/sketch-#{reverse datestring}/)"
eos
end