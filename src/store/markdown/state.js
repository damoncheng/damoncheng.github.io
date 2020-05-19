console.log("markdown state")
const files = require.context(`../../statics/md/`, true, /\.md$/)


const markdown = {};
const markdown_content = {};

files.keys().forEach(item => {
  //modules[item] = files(item).default || files(item)

  let item_arr = item.split('/');
  item_arr.shift();

  markdown_content[item_arr.join('/')] = files(item).default || files(item);

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

console.log(markdown)
console.log(markdown_content)

export default {
  obj: markdown,
  content : markdown_content,
  readme : "Readme.md"
}
