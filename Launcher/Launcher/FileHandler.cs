using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Launcher
{
    class FileHandler
    {
        public FileHandler() { }
        public void Save(string gamer_tag, string ip_adress)
        {
            FileStream fs = new FileStream("temp.txt", FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);

            sw.WriteLine(gamer_tag);
            ip_adress = ip_adress.Trim();
            sw.WriteLine(ip_adress.Trim());
            sw.Flush();
            fs.Close();
        }

    }
}
