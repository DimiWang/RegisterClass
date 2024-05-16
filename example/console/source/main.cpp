#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTextStream>
#include <QFileInfo>
#include "register.h"
#include <QFile>



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("bitfield");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("BitField parser");
    parser.addHelpOption();
    QCommandLineOption verboseOption(QStringList() << "i"<<"input" << "Enable verbose output");
    QCommandLineOption trimOption(QStringList() << "t"<<"file-trim" << "Trim file");
    QCommandLineOption lineEndOption(QStringList() << "e"<<"line-end" << "Line End");
    trimOption.setValueName("file_trim");
    lineEndOption.setValueName("line_end");
    parser.addOption(verboseOption);
    parser.addOption(trimOption);
    parser.addOption(lineEndOption);
    parser.addPositionalArgument("input_file", "Source file to copy.");
    parser.addPositionalArgument("output_file", "Output file");
    parser.addPositionalArgument("format", "Output format.","to_string, to_hex, to_bits, to_u32, to_u8");
    parser.process(app);
    const QStringList args = parser.positionalArguments();

    QString line_end = "\n";
    if(parser.isSet(lineEndOption)){
        const QString line_end_option = parser.value(lineEndOption).trimmed();
        if( line_end_option.contains("<cr>")){
            line_end = "\r";
        }
        if(line_end_option.contains("<lf>")){
            line_end += "\n";
        }        
    }


    if(args.count()>=2 && QFile::exists(args.at(0))){

        const QString format = args[1];
        QFile output;
        if(args.count()>2) output.setFileName(args[2]);
        else{
            output.setFileName(QFileInfo(args[0]).baseName()+".hex");
        }
        QFile input(args[0]);

        if(input.open(QIODevice::ReadOnly) && output.open(QIODevice::WriteOnly))
        {

            Register reg(0,"temporary");            
            if(reg.loadJsonData(input.readAll(),Register::Default1) ==true){

                QRegExp regexp("to_string\\((.*)\\)");
                if(regexp.exactMatch(format)){
                    QTextStream(&output)<<QString(reg.toString(regexp.cap(1)))
                                          .replace("<lf>","\n")
                                          .replace("<cr>","\r");;
                }
                else if( format == "to_string"){
                    QTextStream(&output)<<QString(reg.toString(regexp.cap(1)))
                                          .replace("<lf>","\n")
                                          .replace("<cr>","\r");
                }
                else if(format == "to_u32"){
                    int lines = (reg.size()/32)+(reg.size()%32 != 0);
                    if(parser.isSet(trimOption)){
                        lines = parser.value(trimOption).toUInt();
                    }
                    for(int i=0;i<lines;i++)
                    {
                        const quint32 value = reg.value(i*32,(i+1)*32-1);
                        if(i>0) QTextStream(&output)<<line_end;
                        QTextStream(&output)<<QString("%1").arg(value,8,16,QChar('0'))
                                              .toUpper();
                    }
                }
                else if(format == "to_u8"){
                    int lines = (reg.size()/8)+(reg.size()%8 != 0);
                    if(parser.isSet(trimOption)){
                        lines = parser.value(trimOption).toUInt();

                    }
                    for(int i=0;i<lines;i++)
                    {
                        const quint32 value = reg.value(i*8,(i+1)*8-1);
                        if(i>0) QTextStream(&output)<<line_end;
                        QTextStream(&output)<<QString("%1").arg(value,2,16,QChar('0'))
                                              .toUpper();
                    }
                }
                else if( format == "to_hex"){
                    QTextStream(&output)<<reg.toHex().toUpper();
                }
                else if( format == "to_bits"){
                    QTextStream(&output)<<reg.toBitString();
                }

                output.close();
                input.close();
            }
        }
    }
    return 0;
}
