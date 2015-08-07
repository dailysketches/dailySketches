What it does
--
Allows you to write sketches in openFrameworks, and quickly:
- generate sketches as gifs
- automate the versioning and publishing of those gifs (in a jekyll repo)

Directory Structure
--
This repo's root dir must live alongside openFrameworks in a dir structure like this:

    /dailySketches (this repo's root dir)
        /dailysketches.github.io (jekyll repo root dir)
            /app
                /_posts (jekyll repo posts folder)
                /img
                    /sketches (gif files published here)
        /sketches
            /sketch1 (files will be copied to these dirs when you run 'rake copy')
            /sketch2 (this allows the code to be versioned in a single repo...)
            /etc
        /templates
            /sketchTemplate1
            /sketchTemplate2
            /etc
    /openFrameworks
        /versions
            /084 (root of an openFrameworks 084 instance)
                /dailySketches
                    /sketch1 (this is where you work making your sketches inside the OF dir)
                    /sketch2 (files are copied from these dirs... these files are not directly versioned)
                    /etc
                /dailySketchesTemplates
                    /sketchTemplate1
                    /sketchTemplate2
                    /etc

Usage
--
1. Create a daily sketch in `openFrameworks/084/dailySketches`, based on the GIF template
2. Run the new sketch to generate a GIF in the `bin/data` subdirectory
3. Keep editing and re-running your sketch, it will always overwrite the same file
4. When you are ready to publish, `cd` into the repo root `dailySketches` and run `rake copy`
5. A post is generated here:<br />`dailySketches/dailysketches.github.io/app/_posts`<br />Head over and edit it. Use `grunt serve` to view locally
6. `cd` into `dailysketches.github.io` and run `git status` to ensure everything looks OK
7. To make it live: `git add -A` and `git commit` the changes, then `git push` and `grunt deploy`!