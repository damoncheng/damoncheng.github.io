console.log("markdown state")
const files = require.context(`../../statics/md/`, true, /\.md$/)
const markdown = {};
files.keys().forEach(item => {
  //modules[item] = files(item).default || files(item)
  let item_arr = item.split('/')
  item_arr.shift()

  let domain = null;
  let dmodule = null;
  let section = null;

  if(item_arr.length == 3){

      domain = item_arr[0]
      dmodule = item_arr[1]
      section = item_arr[2]

  }
  else
    return

  if(domain && ! (domain in markdown))
    markdown[domain] = {}

  if(dmodule && ! (dmodule in markdown[domain]))
    markdown[domain][dmodule] = []

  if(section && ! (markdown[domain][dmodule].includes(section)))
    markdown[domain][dmodule].push(section)

})

export default {
  obj: markdown,
  readme : "Readme.md"
}
