using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics.Contracts;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Web;

namespace LittleQuant.Commons.HttpClient
{
    public class SimpleHttpClient : IHttpClient
    {
        private CookieContainer CookieContainer = new CookieContainer();
        private CookieCollection CookieCollection = new CookieCollection();

        public T Get<T>(string uri,
                        NameValueCollection headers = null,
                        SpecialHeaders specialHeaders = null,
                        Func<String, T> deserializer = null,
                        Encoding enc = null)
        {
            enc = enc ?? Encoding.UTF8;

            var req = WebRequest.Create(uri) as HttpWebRequest;
            req.Method = "GET";
            req.CookieContainer = this.CookieContainer;
            if (headers != null)
                req.Headers.Add(headers);
            req.Proxy = null;

            if (specialHeaders != null)
            {
                req.Referer = specialHeaders.Referrer;
            }

            using (HttpWebResponse resp = req.GetResponse() as HttpWebResponse)
            {
                this.CookieCollection = resp.Cookies;
                this.CookieContainer.GetCookies(req.RequestUri);
                var respBody = new Func<string>(() =>
                {
                    using (StreamReader reader = new StreamReader(resp.GetResponseStream(), enc))
                    {
                        return reader.ReadToEnd();
                    }
                }).Invoke();

                if (deserializer == null)
                {
                    if (typeof(T) == typeof(string))
                        return (T)(object)respBody;
                    else
                        throw new ArgumentException();
                }
                else
                    return deserializer(respBody);
            }
        }

        public T Post<T>(string uri,
                         IList<string> form,
                         NameValueCollection headers = null,
                         SpecialHeaders specialHeaders = null,
                         Func<String, T> deserializer = null,
                         Encoding enc = null)
        {
            Contract.Assert(form.Count() % 2 == 0);

            enc = enc ?? Encoding.UTF8;

            var req = WebRequest.Create(uri) as HttpWebRequest;
            req.CookieContainer = this.CookieContainer;
            req.ContentType = "application/x-www-form-urlencoded";
            req.Method = "POST";
            req.Proxy = null;
            req.ProtocolVersion = HttpVersion.Version10;

            if (headers != null)
                req.Headers.Add(headers);
            if (specialHeaders != null)
            {
                req.Referer = specialHeaders.Referrer;
            }

            var formPairs = new List<KeyValuePair<string, string>>(form.Count() / 2);
            for (var i = 0; i < form.Count(); i += 2)
                formPairs.Add(new KeyValuePair<string, string>(form[i], form[i + 1]));
            var postData = new FormUrlEncodedContent(formPairs).ReadAsByteArrayAsync().Result;
            req.ContentLength = postData.Length;
            using (var stream = req.GetRequestStream())
            {
                stream.Write(postData, 0, postData.Length);
            }
            using (var resp = req.GetResponse() as HttpWebResponse)
            {
                this.CookieCollection = resp.Cookies;
                this.CookieContainer.GetCookies(req.RequestUri);
                var respBody = new Func<String>(() =>
                {
                    using (StreamReader reader = new StreamReader(resp.GetResponseStream(), enc))
                    {
                        return reader.ReadToEnd();
                    }
                }).Invoke();

                if (deserializer == null)
                {
                    if (typeof(T) == typeof(string))
                        return (T)(object)respBody;
                    else
                        throw new ArgumentException();
                }
                else
                    return deserializer(respBody);
            }
        }


        public string GetCookie(string name)
        {
            if (CookieCollection[name] != null)
                return CookieCollection[name].Value;

            return null;
        }
    }
}
