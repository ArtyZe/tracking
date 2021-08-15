#include "Main.h"
#include "config.h"

using namespace std;

namespace tld
{

static char help_text[] =
    "usage: MyTLD [option arguments] [arguments]\r\n"
    "option arguments:\r\n"
    "[-i] video path\r\n"
    "[-w] video width to process\r\n"
    "[-h] video height to process\r\n"
    "[-t] threahold to detect positive\r\n"
    "[-minwin] min window to capture postive box\r\n"
    "[-minscale] min scale to process\r\n"
    "[-maxscale] max scale to process\r\n"
    "[-learnEn] machine learning enable\r\n"
    "[-shiftEn] shift enable\r\n"
    "[-shift] proportional shift\r\n"
    "[-trackEn] tracking enable\r\n"
    "[-vfilterEn] varianceFilter enable\r\n"
    "[-esClassEn] ensembleClassifier enabled\r\n"
    "[-nnClassEn] nnClassifier enable\r\n"
    "[-nTrees] random Forests trees nums\r\n"
    "[-nFeatures] Features num\r\n"
    "[-p] positive threshold\r\n"
    "[-n] negitive threshold\r\n"
    "[-trajectory] specifies the number of the last frames which are considered by the trajectory; 0 disables the trajectory\r\n";
/*
"usage: tld [option arguments] [arguments]\n"
"option arguments:\n"
"[-a <startFrameNumber>] video starts at the frameNumber <startFrameNumber>\n"
"[-b <x,y,w,h>] Initial bounding box\n"
"[-c] shows color images instead of greyscale\n"
"[-d <device>] select input device: <device>=(IMGS|CAM|VID)\n"
"    IMGS: capture from images\n"
"    CAM: capture from connected camera\n"
"    VID: capture from a video\n"
"[-e <path>] export model after run to <path>\n"
"[-f] shows foreground\n"
"[-i <path>] <path> to the images or to the video\n"
"[-h] shows help\n"
"[-j <number>] specifies the <number> of the last frames which are considered by the trajectory; 0 disables the trajectory\n"
"[-m <path>] if specified load a model from <path>. An initialBoundingBox must be specified or selectManually must be true.\n"
"[-n <number>] specifies which camera device to use.\n"
"[-p <path>] prints results into the file <path>\n"
"[-q] open QT-Config GUI\n"
"[-s] if set, user can select initial bounding box\n"
"[-t <theta>] threshold for determining positive results\n"
"[-z <lastFrameNumber>] video ends at the frameNumber <lastFrameNumber>.\n"
"    If <lastFrameNumber> is 0 or the option argument isn't specified means\n"
"    take all frames.\n"
"arguments:\n"
"[<path>] <path> to the config file\n";
*/

Config::Config() :
    m_qtConfigGui(false),
    m_selectManuallySet(false),
    m_methodSet(false),
    m_startFrameSet(false),
    m_lastFrameSet(false),
    m_trajectorySet(false),
    m_showDetectionsSet(false),
    m_showForegroundSet(false),
    m_thetaSet(false),
    m_printResultsSet(false),
    m_camNoSet(false),
    m_imagePathSet(false),
    m_modelPathSet(false),
    m_initialBBSet(false),
    m_showOutputSet(false),
    m_exportModelAfterRunSet(false),
    m_width(0),
    m_height(0)
{
}

Config::~Config()
{}

void Config::printinfo()
{
    printf(help_text);
}

int Config::init(int argc, char ** argv)
{
    // 1 ≤Œ ˝…Ë÷√
    if (argc < 2)
    {
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        // video path input
        if (strcmp(argv[i], "-i") == 0)
        {
            m_settings.m_imagePath = argv[i + 1];
        }
        // video width
        else if (strcmp(argv[i], "-w") == 0)
        {
            m_width = atoi(argv[i + 1]);

            if (m_width == 0)
            {
                return 0;
            }
        }
        // video height
        else if (strcmp(argv[i], "-h") == 0)
        {
            m_height = atoi(argv[i + 1]);

            if (m_height == 0)
            {
                return 0;
            }
        }
        // threshold for determining positive results
        else if (strcmp(argv[i], "-t") == 0)
        {
            m_settings.m_threshold = (float)atof(argv[i + 1]);
        }
        // minwindow size
        else if (strcmp(argv[i], "-minwin") == 0)
        {
            m_settings.m_minSize = atoi(argv[i + 1]);
        }
        // min scale
        else if (strcmp(argv[i], "-minscale") == 0)
        {
            m_settings.m_minScale = atoi(argv[i + 1]);
        }
        // max scale
        else if (strcmp(argv[i], "-maxscale") == 0)
        {
            m_settings.m_maxScale = atoi(argv[i + 1]);
        }
        // learning Enable
        else if (strcmp(argv[i], "-learnEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_learningEnabled = true;
            }
            else
            {
                m_settings.m_learningEnabled = false;
            }
        }
        // useProportionalShift
        else if (strcmp(argv[i], "-shiftEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_useProportionalShift = true;
            }
            else
            {
                m_settings.m_useProportionalShift = false;
            }
        }
        // proportional shift
        else if (strcmp(argv[i], "-shift") == 0)
        {
            m_settings.m_proportionalShift = (float)atof(argv[i + 1]);
        }
        // trackerEnabled
        else if (strcmp(argv[i], "-trackEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_trackerEnabled = true;
            }
            else
            {
                m_settings.m_trackerEnabled = false;
            }
        }
        // varianceFilterEnabled
        else if (strcmp(argv[i], "-vfilterEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_varianceFilterEnabled = true;
            }
            else
            {
                m_settings.m_varianceFilterEnabled = false;
            }
        }
        // m_ensembleClassifierEnabled
        else if (strcmp(argv[i], "-esClassEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_ensembleClassifierEnabled = true;
            }
            else
            {
                m_settings.m_ensembleClassifierEnabled = false;
            }
        }
        // m_nnClassifierEnableds
        else if (strcmp(argv[i], "-nnClassEn") == 0)
        {
            if (atoi(argv[i + 1]))
            {
                m_settings.m_nnClassifierEnabled = true;
            }
            else
            {
                m_settings.m_nnClassifierEnabled = false;
            }
        }
        // Trees num
        else if (strcmp(argv[i], "-nTrees") == 0)
        {
            m_settings.m_numTrees = atoi(argv[i + 1]);
        }
        // Features num
        else if (strcmp(argv[i], "-nFeatures") == 0)
        {
            m_settings.m_numFeatures = atoi(argv[i + 1]);
        }
        // pos threshold
        else if (strcmp(argv[i], "-p") == 0)
        {
            m_settings.m_thetaP = (float)atof(argv[i + 1]);
        }
        // neg threshold
        else if (strcmp(argv[i], "-n") == 0)
        {
            m_settings.m_thetaN = (float)atof(argv[i + 1]);
        }
        // trajectory
        else if (strcmp(argv[i], "-trajectory") == 0)
        {
            m_settings.m_trajectory = atoi(argv[i + 1]);
        }
    }

    // method
    m_settings.m_method = IMACQ_VID; // use video

    // startFrame
    m_settings.m_startFrame = 100;

    // lastFrame
    m_settings.m_lastFrame = 0;

    // FPS
    m_settings.m_fps = 40;

    // camNo
    m_settings.m_camNo = 0;

    // selectManually
    m_settings.m_selectManually = true;

    // showNotConfident
    m_settings.m_showNotConfident = true;

    // showForeground
    m_settings.m_showForeground = true;

    // showDetections
    m_settings.m_showDetections = true;

    // alternating
    m_settings.m_alternating = false;

    // seed
    m_settings.m_seed = 0;

    // init bounding box
    m_settings.m_initialBoundingBox.clear();

    return SUCCESS;
}

int Config::configure(Main* main)
{
    ImAcq* imAcq = main->imAcq;

    // imAcq
    imAcq->method = m_settings.m_method;
    imAcq->imgPath = (m_settings.m_imagePath.empty()) ? NULL : m_settings.m_imagePath.c_str();
    imAcq->lastFrame = m_settings.m_lastFrame;
    imAcq->currentFrame = m_settings.m_startFrame;
    imAcq->camNo = m_settings.m_camNo;
    imAcq->fps = m_settings.m_fps;
    imAcq->width = m_width; // video resize width
    imAcq->height = m_height; // vidoe resize height

    // main
    main->tld->trackerEnabled = m_settings.m_trackerEnabled;
    main->showOutput = m_settings.m_showOutput;
    main->printResults = (m_settings.m_printResults.empty()) ? NULL : m_settings.m_printResults.c_str();
    main->saveDir = (m_settings.m_outputDir.empty()) ? NULL : m_settings.m_outputDir.c_str();
    main->threshold = m_settings.m_threshold;
    main->showForeground = m_settings.m_showForeground;
    main->showNotConfident = m_settings.m_showNotConfident;
    main->tld->alternating = m_settings.m_alternating;
    main->tld->learningEnabled = m_settings.m_learningEnabled;
    main->selectManually = m_settings.m_selectManually;
    main->exportModelAfterRun = m_settings.m_exportModelAfterRun;
    main->modelExportFile = m_settings.m_modelExportFile.c_str();
    main->loadModel = m_settings.m_loadModel;
    main->modelPath = (m_settings.m_modelPath.empty()) ? NULL : m_settings.m_modelPath.c_str();
    main->seed = m_settings.m_seed;

    if (m_settings.m_initialBoundingBox.size() > 0)
    {
        main->initialBB = new int[4];

        for (int i = 0; i < 4; i++)
        {
            main->initialBB[i] = m_settings.m_initialBoundingBox[i];
        }
    }

    DetectorCascade* detectorCascade = main->tld->detectorCascade;
    detectorCascade->varianceFilter->enabled = m_settings.m_varianceFilterEnabled;
    detectorCascade->ensembleClassifier->enabled = m_settings.m_ensembleClassifierEnabled;
    detectorCascade->nnClassifier->enabled = m_settings.m_nnClassifierEnabled;

    // classifier
    detectorCascade->useShift = m_settings.m_useProportionalShift;
    detectorCascade->shift = m_settings.m_proportionalShift;
    detectorCascade->minScale = m_settings.m_minScale;
    detectorCascade->maxScale = m_settings.m_maxScale;
    detectorCascade->minSize = m_settings.m_minSize;
    detectorCascade->numTrees = m_settings.m_numTrees;
    detectorCascade->numFeatures = m_settings.m_numFeatures;
    detectorCascade->nnClassifier->thetaTP = m_settings.m_thetaP;
    detectorCascade->nnClassifier->thetaFP = m_settings.m_thetaN;

    return SUCCESS;
}

/*
POSIX getopt for Windows

AT&T Public License

Code given out at the 1985 UNIFORUM conference in Dallas.
*/
#ifndef __GNUC__

#define NULL    0
#define EOF (-1)
#define ERR(s, c)   if(opterr){\
        char errbuf[2];\
        errbuf[0] = c; errbuf[1] = '\n';\
        fputs(argv[0], stderr);\
        fputs(s, stderr);\
        fputc(c, stderr);}

int opterr = 1;
int optind = 1;
int optopt;
char *optarg;

int getopt(int argc, char **argv, char *opts)
{
    static int sp = 1;
    register int c;
    register char *cp;

    if (sp == 1)
        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
        {
            return (EOF);
        }
        else if (strcmp(argv[optind], "--") == NULL)
        {
            optind++;
            return (EOF);
        }

    optopt = c = argv[optind][sp];

    if (c == ':' || (cp = strchr(opts, c)) == NULL)
    {
        ERR(": illegal option -- ", c);

        if (argv[optind][++sp] == '\0')
        {
            optind++;
            sp = 1;
        }

        return ('?');
    }

    if (*++cp == ':')
    {
        if (argv[optind][sp + 1] != '\0')
        {
            optarg = &argv[optind++][sp + 1];
        }
        else if (++optind >= argc)
        {
            ERR(": option requires an argument -- ", c);
            sp = 1;
            return ('?');
        }
        else
        {
            optarg = argv[optind++];
        }

        sp = 1;
    }
    else
    {
        if (argv[optind][++sp] == '\0')
        {
            sp = 1;
            optind++;
        }

        optarg = NULL;
    }

    return (c);
}

#endif  /* __GNUC__ */

}
