#include "../include/base_lib/BinaryStream.h"
#include "../include/base_lib/Color.h"
#include "../include/base_lib/Color.h"
#include "../include/base_lib/File.h"
#include "../include/base_lib/performance.h"
#include "../include/base_lib/Random.h"

int main(int argc, char* argv[])
{
    Utils::init_base_lib(argv[0]);
    
    Compound::Object obj1 = Compound::Convert::JSON().parse_value(File::read_file("test.json")).get_object();

    auto jconv = Compound::Convert::JSON();
    jconv.separate_with_new_line = true;
    File::write_file("test_.json", jconv.format_value(obj1));

    if (auto writer = BinaryWriter::open("test.cmp"))
    {
        writer->write(obj1);
        
        writer->close();
    }

    Compound::Object obj2;
    if (auto reader = BinaryReader::open("test.cmp"))
    {
        obj2 = reader->read<Compound::Object>();
    }

    File::write_file("test.yaml", Compound::Convert::YAML().format_value(obj1));
    
    return 0;
}
