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
        public void Save(string gamer_tag, string ip_adress, string res)
        {
            FileStream fs = new FileStream("temp.txt", FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);

            string res_x;
            string res_y;

            res_x = res.Substring(0, res.IndexOf(' '));
            res_y = res.Substring(res.LastIndexOf(' ')+1);

            sw.WriteLine(gamer_tag);
            ip_adress = ip_adress.Trim();
            sw.WriteLine(ip_adress.Trim());
            sw.WriteLine(res_x);
            sw.WriteLine(res_y);
            sw.Flush();
            fs.Close();
        }

    }
}
