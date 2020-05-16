<template>
    <div>

        <!--
            显示section内容
        -->
        <Markdown :markdown_name="markdown_name">
        </Markdown>

    </div>
</template>

<script>

import Markdown from "components/Markdown";

export default {
  name: 'section',
  components: {
      Markdown : Markdown
  },
  data () {
    return {
        domain : this.$route.params.domain,
        module: this.$route.params.module,
        section : this.$route.params.section
    }
  },
  methods: {
    
  },
  computed: {

      markdown_name () {
          return `${this.domain}/${this.module}/${this.section}.md`
      }

  },
  updated (){

    
  }
}
</script>

<style>
</style>
