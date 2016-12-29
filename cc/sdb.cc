#include <limits>

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

class CoordinatesHandler : public HandlerBase
{
 public:
    inline CoordinatesHandler(Chart& aTarget, Location& aField) : HandlerBase{aTarget}, mElement(0), mField(aField) {}

    inline virtual HandlerBase* Double(double d)
        {
            switch (mElement) {
              case 0:
                  mField.x = d;
                  break;
              case 1:
                  mField.y = d;
                  break;
              default:
                  throw json_reader::Failure{};
            }
            ++mElement;
            return nullptr;
        }

    inline virtual HandlerBase* Null()
        {
            switch (mElement) {
              case 0:
                  mField.x = std::numeric_limits<double>::quiet_NaN();
                  break;
              case 1:
                  mField.y = std::numeric_limits<double>::quiet_NaN();
                  break;
              default:
                  throw json_reader::Failure{};
            }
            ++mElement;
            return nullptr;
        }

 private:
    size_t mElement;
    Location& mField;
};

// ----------------------------------------------------------------------

class PointAttributesHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, antigen_serum, egg, reassortant, reference, vaccine, homologous_antigen, homologous_titer, serum_circle_radius};

 public:
    inline PointAttributesHandler(Chart& aChart, PointAttributes& aField) : HandlerBase{aChart}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
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
              case Keys::homologous_titer:
                  mField.homologous_titer.assign(str, length);
                  break;
              case Keys::antigen_serum:
                  if (length == 1 && (*str == 'a' || *str == 's'))
                      mField.antigen = *str == 'a';
                  else
                      throw json_reader::Failure("Unrecognized point type (a or s expected)");
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Uint(unsigned u)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::homologous_antigen:
                  mField.homologous_antigen = static_cast<int>(u);
                  break;
              case Keys::reference:
                  mField.reference = u != 0;
                  break;
              default:
                  result = HandlerBase::Uint(u);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::serum_circle_radius:
                  mField.serum_circle_radius = d;
                  break;
              default:
                  result = HandlerBase::Double(d);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Bool(bool b)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::egg:
                  mField.egg = b;
                  break;
              case Keys::reassortant:
                  mField.reassortant = b;
                  break;
              case Keys::reference:
                  mField.reference = b;
                  break;
              case Keys::vaccine:
                  mField.vaccine = b;
                  break;
              default:
                  result = HandlerBase::Bool(b);
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    PointAttributes& mField;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, PointAttributesHandler::Keys> PointAttributesHandler::key_mapper {
    {"t", Keys::antigen_serum},
    {"e", Keys::egg},
    {"r", Keys::reassortant},
    {"R", Keys::reference},
    {"v", Keys::vaccine},
    {"ha", Keys::homologous_antigen},
    {"ht", Keys::homologous_titer},
    {"ra", Keys::serum_circle_radius}
};

// ----------------------------------------------------------------------

class PointHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, name, coordinates, lab_id, attributes};

 public:
    inline PointHandler(Chart& aChart, Point& aField) : HandlerBase{aChart}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
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
              case Keys::name:
                  mField.name.assign(str, length);
                  break;
              case Keys::lab_id:
                  mField.lab_id.assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::attributes:
                  result = new PointAttributesHandler(mTarget, mField.attributes);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* StartArray()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::coordinates:
                  result = new CoordinatesHandler(mTarget, mField.coordinates);
                  break;
              default:
                  result = HandlerBase::StartArray();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    Point& mField;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, PointHandler::Keys> PointHandler::key_mapper {
    {"N", Keys::name},
    {"c", Keys::coordinates},
    {"l", Keys::lab_id},
    {"a", Keys::attributes}
};

// ----------------------------------------------------------------------

class PointStyleHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, fill_color, outline_color, aspect, shape, size, rotation};

 public:
    inline PointStyleHandler(Chart& aChart, PointStyle& aField) : HandlerBase{aChart}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
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
              case Keys::fill_color:
                  mField.fill_color.from_string(str, length);
                  break;
              case Keys::outline_color:
                  mField.outline_color.from_string(str, length);
                  break;
              case Keys::shape:
                  mField.shape.assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mKey) {
              case Keys::aspect:
                  mField.aspect = d;
                  break;
              case Keys::size:
                  mField.size = d;
                  break;
              case Keys::rotation:
                  mField.rotation = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

 private:
    Keys mKey;
    PointStyle& mField;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, PointStyleHandler::Keys> PointStyleHandler::key_mapper {
    {"fill_color", Keys::fill_color},
    {"outline_color", Keys::outline_color},
    {"aspect", Keys::aspect},
    {"shape", Keys::shape},
    {"size", Keys::size},
    {"rotation", Keys::rotation}
};

// ----------------------------------------------------------------------

class PlotStyleHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, points, styles};

 public:
    inline PlotStyleHandler(Chart& aChart) : HandlerBase{aChart}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::points:
                      result = new json_reader::UintListHandler<Chart>(mTarget, mTarget.plot_style().points);
                      break;
                  case Keys::styles:
                      result = new json_reader::ListHandler<Chart, PointStyle, PointStyleHandler>(mTarget, mTarget.plot_style().styles);
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

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, PlotStyleHandler::Keys> PlotStyleHandler::key_mapper {
    {"points", Keys::points},
    {"styles", Keys::styles}
};

// ----------------------------------------------------------------------

class ChartInfoHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, date, lab, virus_type, lineage, name, rbc_species};

 public:
    inline ChartInfoHandler(Chart& aChart) : HandlerBase{aChart}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
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
              case Keys::date:
                  mTarget.chart_info().date.assign(str, length);
                  break;
              case Keys::lab:
                  mTarget.chart_info().lab.assign(str, length);
                  break;
              case Keys::virus_type:
                  mTarget.chart_info().virus_type.assign(str, length);
                  break;
              case Keys::lineage:
                  mTarget.chart_info().lineage.assign(str, length);
                  break;
              case Keys::name:
                  mTarget.chart_info().name.assign(str, length);
                  break;
              case Keys::rbc_species:
                  mTarget.chart_info().rbc_species.assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Null()
        {
            return nullptr;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, ChartInfoHandler::Keys> ChartInfoHandler::key_mapper {
    {"date", Keys::date},
    {"lab", Keys::lab},
    {"virus_type", Keys::virus_type},
    {"lineage", Keys::lineage},
    {"name", Keys::name},
    {"rbc_species", Keys::rbc_species}
};

// ----------------------------------------------------------------------

class TransformationHandler : public HandlerBase
{
 public:
    inline TransformationHandler(Chart& aChart, Transformation& aField) : HandlerBase{aChart}, mField(aField), mNesting(0) { mField.clear(); }

    inline virtual HandlerBase* StartArray()
        {
            switch (mNesting) {
              case 0:
              case 1:
                  ++mNesting;
                  break;
              default:
                  throw json_reader::Failure{};
            }
            return nullptr;
        }

    inline virtual HandlerBase* EndArray()
        {
            if (mNesting == 0)
                throw json_reader::Failure{"Unexpected ] while reading transformation"};
            if (--mNesting == 0) {
                if (mField.size() == 4)
                    throw json_reader::Pop();
                else
                    throw json_reader::Failure{"Not enough elements of transformation read"};
            }
            return nullptr;
        }

    inline virtual HandlerBase* Double(double d)
        {
            if (mNesting != 2 || mField.size() >= 4)
                throw json_reader::Failure{};
            mField.push_back(d);
            return nullptr;
        }

    inline virtual HandlerBase* EndObject() { throw json_reader::Failure(); }

 private:
    Transformation& mField;
    size_t mNesting;
};

// ----------------------------------------------------------------------

class ChartRootHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, version, created, info, minimum_column_basis, points, stress, column_bases, transformation, plot, drawing_order /*, intermediate_layouts */};

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
                  case Keys::drawing_order:
                      result = new json_reader::UintListHandler<Chart>(mTarget, mTarget.drawing_order());
                      break;
                  case Keys::column_bases:
                      result = new json_reader::DoubleListHandler<Chart>(mTarget, mTarget.column_bases());
                      break;
                  case Keys::points:
                      result = new json_reader::ListHandler<Chart, Point, PointHandler>(mTarget, mTarget.points());
                      break;
                  case Keys::transformation:
                      result = new TransformationHandler(mTarget, mTarget.transformation());
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
              case Keys::minimum_column_basis:
                  mTarget.minimum_column_basis().assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::info:
                  result = new ChartInfoHandler(mTarget);
                  break;
              case Keys::plot:
                  result = new PlotStyleHandler(mTarget);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mKey) {
              case Keys::stress:
                  mTarget.stress(d);
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, ChartRootHandler::Keys> ChartRootHandler::key_mapper {
    {"  version", Keys::version},
    {" created", Keys::created},
    {"drawing_order", Keys::drawing_order},
    {"info", Keys::info},
    {"minimum_column_basis", Keys::minimum_column_basis},
    {"plot", Keys::plot},
    {"points", Keys::points},
    {"stress", Keys::stress},
    {"transformation", Keys::transformation},
              // {"intermediate_layouts", Keys::intermediate_layouts},
    {"column_bases", Keys::column_bases},
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
