#include "chart.hh"
#include "acmacs-base/json-reader.hh"

// ----------------------------------------------------------------------

constexpr const char* SDB_VERSION = "acmacs-sdb-v2";

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

using HandlerBase = json_reader::HandlerBase<Chart>;

// ----------------------------------------------------------------------

class ChartRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, version, created, intermediate_layouts, info, minimum_column_basis, points, stress, column_bases, transformation, plot, drawing_order};

 public:
    inline ChartRootHandler(Chart& aChart) : HandlerBase{aChart}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::created:
                      mIgnore = true;
                      break;
                  default:
                      break;
                }
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::version:
                  if (strncmp(str, SDB_VERSION, std::min(length, static_cast<rapidjson::SizeType>(strlen(SDB_VERSION))))) {
                      std::cerr << "Unsupported version: \"" << std::string(str, length) << '"' << std::endl;
                      throw json_reader::Failure();
                  }
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

        // inline virtual HandlerBase* StartObject()
        //     {
        //         HandlerBase* result = nullptr;
        //         switch (mKey) {
        //           case Keys::signature_page:
        //               result = new SignaturePageDrawChartHandler(mTarget);
        //               break;
        //           case Keys::tree:
        //               result = new TreeDrawChartHandler(mTarget);
        //               break;
        //           case Keys::time_series:
        //               result = new TimeSeriesDrawChartHandler(mTarget);
        //               break;
        //           case Keys::clades:
        //               result = new CladesDrawChartHandler(mTarget);
        //               break;
        //           case Keys::hz_sections:
        //               result = new HzSectionsHandler(mTarget);
        //               break;
        //           default:
        //               result = HandlerBase::StartObject();
        //               break;
        //         }
        //         return result;
        //     }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, ChartRootHandler::Keys> ChartRootHandler::key_mapper {
    {"  version", Keys::version},
    {" created", Keys::created},
    {"intermediate_layouts", Keys::intermediate_layouts},
    {"info", Keys::info},
    {"minimum_column_basis", Keys::minimum_column_basis},
    {"points", Keys::points},
    {"stress", Keys::stress},
    {"column_bases", Keys::column_bases},
    {"transformation", Keys::transformation},
    {"plot", Keys::plot},
    {"drawing_order", Keys::drawing_order}
};

// ----------------------------------------------------------------------

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

void read_chart_from_sdb(Chart& aChart, std::string aFilename)
{
    json_reader::read_from_file<Chart, ChartRootHandler>(aFilename, aChart);

} // read_chart_from_sdb

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
