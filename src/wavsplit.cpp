#include "common.h"
#include "division.h"
#include "wavsplit.h"
#include <sndfile.hh>
#include <wx/filename.h>

std::vector<double> GetSplitPoints(Division *pDivision, double bpm, double offset_ms);

void SplitFile(
        const wxString &infile,
        const wxString &outpath,
        const wxString& wav_prefix,
        Division* div,
        double bpm,
        double offset_ms
) {
    SndfileHandle wav(infile);

    if (!wav) {
        throw std::runtime_error(wxString::Format(_("Couldn't read input audio file '%s'"), infile));
    }

    wxFileName inpath(infile);
    auto in_extension = inpath.GetExt();

    wxFileName path(outpath);

    if (!path.IsDir())
        throw std::runtime_error(_("Output path is not a directory"));

    if (!path.DirExists())
        throw std::runtime_error(_("Output path does not exist"));

    if (!path.IsDirWritable())
        throw std::runtime_error(_("Output path is not writeable"));

    std::vector<double> time = GetSplitPoints(div, bpm, offset_ms);
    std::sort(time.begin(), time.end()); /* just in case */

    for (int i = 0; i < time.size(); i++) {
        auto time_start = time[i];
        double time_end;

        if (i + 1 < time.size()) { /* get the next event time */
            time_end = time[i + 1];
        } else {
            time_end = (double)wav.frames() / (double)wav.samplerate();
        }

        /* go to start time */
        wav.seek(time_start * wav.samplerate(), SEEK_SET);

        /* start writing file out */
        auto fn = wxString::Format("%s/%s_%03d.wav", outpath, wav_prefix, i + 1);
        SndfileHandle wav_out(fn, SFM_WRITE, wav.format(), wav.channels(), wav.samplerate());

        if (!wav_out) {
            throw std::runtime_error(wxString::Format(_("Couldn't open %s for output."), fn));
        }

        /* duration of this splice is given by time_start and time_end */
        auto frame_count = (time_end - time_start) * wav.samplerate();

        /* allocate temporary storage for our read data */
        std::vector<float> samp_data(frame_count * wav.channels());

        /* copy it out */
        wav.readf(samp_data.data(), frame_count);
        wav_out.writef(samp_data.data(), frame_count);
    }
}

std::vector<double> GetSplitPoints(Division *pDivision, double bpm, double offset_ms) {
    std::set<double> ret;

    for (int i = 0; i < pDivision->notes_count(); i++) {
        auto note = pDivision->notes(i);
        auto beat = (double)note.position / (double)pDivision->get_quantize();
        auto spb = 60 / bpm;
        auto time = spb * beat + offset_ms / 1000.0;

        ret.insert(time);
    }

    return std::vector(ret.begin(), ret.end());
}
