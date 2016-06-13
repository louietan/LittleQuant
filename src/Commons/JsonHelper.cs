using Newtonsoft.Json;

namespace LittleQuant.Commons
{
    public static class JsonHelper
    {
        public static string ToJson(object value, string dateFormat = null)
        {
            var settings = new JsonSerializerSettings { ReferenceLoopHandling = ReferenceLoopHandling.Ignore };
            if (dateFormat != null)
                settings.DateFormatString = dateFormat;
            return JsonConvert.SerializeObject(value, settings);
        }

        public static T ToObject<T>(string json)
        {
            return JsonConvert.DeserializeObject<T>(json);
        }
    }
}
