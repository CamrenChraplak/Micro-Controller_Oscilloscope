#Stores working directories
#webDir = "data/WebServer/"
webDir = "webserver/WebServer/"
srcDir = "src/"

index = open(srcDir + "index.h", "w")
html = open(webDir + "index.html", "r")
format = open("webserver/format.html", "w")

lines = html.readlines()
index.write('const char webpage[] PROGMEM = R"=====(\n')

#Writes to final header and html file
def writer(text):
    index.write(text)
    format.write(text)

#Loops through index.html
for line in lines:
    #Tests if there is a style document
    if line.find('.css') != -1:

        begin = line.find('"')
        end = line.find('.css')

        #Opens the style file
        css = open(webDir + line[begin+1:end+4], "r")
        cssLines = css.readlines()
        writer('\t<style type="text/css">\n')

        #Copies text in stlye file to html
        for cssLine in cssLines:
            writer('\t\t' + cssLine)
        writer('\n\t</style>\n')
        css.close()

    #Tests if there is a javascript document
    elif line.find('.js') != -1:

        begin = line.find('"')
        end = line.find('.js')

        #Opens the javascript file
        js = open(webDir + line[begin+1:end+3], "r")
        jsLines = js.readlines()
        writer('\t<script>\n')

        #Loops through the text in javascript file
        for jsLine in jsLines:

            #Checks if there is a webworker file
            if jsLine.find('.js') != -1:
                begin = jsLine.find('\'')
                end = jsLine.find('.js')

                #Opens the web worker file
                work = open(webDir + jsLine[begin+1:end+3], "r")
                workLines = work.readlines()
                writer('\t\tlet ' + jsLine[begin+1:end] + 'Worker = new makeWorker(`\n')

                #Copies web worker file contents into javascript file
                for workLine in workLines:
                    writer('\t\t\t' + workLine.replace('\t', ''))
                writer('\n\t\t`);\n')
                work.close()

            else:
                #Copies text in javascript file to html
                writer('\t\t' + jsLine)
        
        js.close()
        writer('\n\t</script>\n')

    else:
        writer(line)

index.write(')=====";')
index.close()
format.close()
html.close()