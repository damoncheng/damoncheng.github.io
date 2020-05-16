<template>
    <div>
        <span v-html="markdown_content"></span>

    </div>
    
</template>

<script>

    import { Remarkable } from 'remarkable';

    export default {
        props: ['markdown_name'],
        data () {
            return {
                markdown_content : "loading...",
                markdown_href_list : [],
                api : "/statics/md/" + this.markdown_name,
            }
        },
        computed : {
            check_markdown_href_list ()  {

                console.log(this.markdown_href_list.length > 0)
                return this.markdown_href_list.length > 0

            }
        },
        methods : {
            
            load_markdown () {

                console.log(this.api)
                this.$axios.get(this.api).then((response) => {
                    this.markdown_content = new Remarkable().render(response.data)
                })

            }
        },
        mounted() {
            
            this.load_markdown()
        }
    }
</script>

 <style scoped>

    div {
        margin:10px
    }

    h1,h2,h3,h4,h5,h6 {
       font-size: auto;
    }

</style>
